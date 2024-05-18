#include <iostream>
#include <string>
#include <memory>
#include <ctime>
#include <thread>

#include <ur_client_library/rtde/rtde_client.h>
#include <database.h>
#include <motor.h>
#include <Encoder.h>
#include "AmMeter.h"
#include <wiringPiSPI.h>

using namespace urcl;

const std::string DEFAULT_ROBOT_IP = "192.168.1.54";
const std::string OUTPUT_RECIPE = "/home/pi/GripperRobot/rtde_output_recipe.txt";
const std::string INPUT_RECIPE = "/home/pi/GripperRobot/rtde_input_recipe.txt";
const std::chrono::milliseconds READ_TIMEOUT{ 1000 };


int main()
{

  wiringPiSetupGpio(); //initializer for gpio.

  comm::INotifier notifier;
  rtde_interface::RTDEClient my_client(DEFAULT_ROBOT_IP, notifier, OUTPUT_RECIPE, INPUT_RECIPE);
  Encoder enc;
  Motor m;
  AmMeter AM;
  Database DB;
  //DB.createDatabase("GriberData");
  DB.useDatabase("GriberData");
  //DB.createTableGreb("Greb");
  //DB.createTablePose("Pose");


  my_client.init();
  my_client.start();


  static bool RobotInPickPos127 = 0;
  static bool RobotInPlacePos126 = 0;
  static bool RobotReady125 = 0;
  static int32_t BoxSizeToRobot47 = 0;
  static std::array<double, 6> PoseOnGrip{};
  static float adc_1;
  static bool flag = false;
  static int tempEnc = 0;



auto getRobotRTDE = [&](){
    while (true)
    {
        std::unique_ptr<rtde_interface::DataPackage> data_pkg = my_client.getDataPackage(READ_TIMEOUT);
        if (data_pkg)
        {

            data_pkg->getData("output_bit_register_127", RobotInPickPos127);
            data_pkg->getData("output_bit_register_126", RobotInPlacePos126);
            data_pkg->getData("output_bit_register_125", RobotReady125);
            data_pkg->getData("actual_q", PoseOnGrip);
            //std::cout << "Robot in pick: " << RobotInPickPos127 << std::endl;
            //std::cout << "Robot in place: " << RobotInPlacePos126 << std::endl;
            //std::cout << "Robot ready?: "  << RobotReady125 << std::endl;
        }

        else
        {
        std::cout << "Could not get fresh data package from robot" << std::endl;
        }
    }
};

auto getVoltage = [&](){
    while(true){
        if(!flag){
            try {
                adc_1 = AM.getADC(1);
                //std::cout << "Spænding over modstand på kanal 1: " << adc_1 << " VDC" << std::endl;
                usleep(50000);
            } catch (...) {
            // Catch all exceptions to ensure cleanup
            std::cerr << "An error occurred." << std::endl;
            }
        }
    }
};

auto getEncoder = [&](){
    while(true){
        tempEnc = enc.getOrientation();
        enc.updateCounter();
        if(enc.getOrientation() != tempEnc)
            std::cout << "Position: " << enc.getOrientation() << std::endl;
    }
};

std::jthread dataThread1(getRobotRTDE);
std::jthread dataThread2(getVoltage);
std::jthread dataThread3(getEncoder);

int RP = 1;

while(true){
    // switch case
    switch (RP) {

    //Homing/Nulstillings case
    case 1:
        //Åben griber kort, sikkerhed til strømmåling
        delay(500);

        my_client.getWriter().sendInputBitRegister(127, 0);
        my_client.getWriter().sendInputBitRegister(126, 0);
        my_client.getWriter().sendInputBitRegister(125, 0);

        std::cout << "In case: " << RP << std::endl;
        std::cout << "Sikkerheds åbning!" << std::endl;
        m.setSpeed(200);
        m.setDirection(0);
        delay(100);
        m.startMotor();
        delay(250);
        m.stopMotor();
        delay(1000);
        //Ændre retning og start motor
        std::cout << "Lukker mod nulstilling!" << std::endl;
        m.setDirection(1);
        delay(100);
        m.startMotor();
        delay(600);
        //While til overvågning af strømmen. Delay på 600ms inden overvågning start, for at undvige startstrøm
        while(true){
            if(adc_1 > 4.8)
            {
                //Ved opnåelse af lukkestrøm, stop motor, ændre retning og slip "stresset" i griber, nulstil encoder, skift state.
                std::cout << "Center nået, nulstil encoder!" << std::endl;
                m.stopMotor();
                delay(500);
                m.setSpeed(50);
                m.setDirection(0);
                delay(100);
                m.startMotor();
                delay(300);
                m.stopMotor();
                delay(1000);
                enc.setOrientation(0);
                RP = 9;
                std::cout << enc.getOrientation() << std::endl;
                break;
            }
        }

        break;

    //Åben case efter nulstilling
    case 9:
        //Kør griber helt åben
        std::cout << "In case: " << RP << std::endl;
        m.setSpeed(200);
        m.setDirection(0);
        delay(1000);
        m.startMotor();
        std::cout << "Køre mod endestop!" << std::endl;
        //Ved den korrekte encoder værdier, stop griber hårdt med retningsskift for at modvirke drift på encoder
        while(true){
            if(enc.getOrientation() >= 19){
                //enc.setOrientation(19);
                std::cout << "Nået endestop!" << std::endl;
                m.stopMotor();
                delay(1);
                m.setSpeed(100);
                m.setDirection(1);
                delay(10);
                m.startMotor();
                delay(100);
                m.stopMotor();
                RP = 10;
                break;
            }
        }
        break;

    //Ready case, venter på robot
    case 10:
        //Afvent at BOOL skifter værdi, når robot klar til kørsel
        if (RobotReady125) {

            my_client.getWriter().sendInputBitRegister(127, 1);
            RP = 15;
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(127, 0);

        //std::cout << PoseOnGrip << std::endl;
        //usleep(50000);
        break;

    case 15:
        //Når robotten er i opsamlingsposition, luk griber og grib kasse
        if (RobotInPickPos127)
        {
            std::cout << "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" << std::endl;
            //Samme lukke fremgangsmåde som i case 1
            m.setSpeed(200);
            m.setDirection(1);
            delay(500);
            std::cout << "Start motor luk!" << std::endl;
            m.startMotor();
            delay(600);

            while(true){

                //Ved greb af kasse, aflæs encoder og gem i database
                if(adc_1 > 4.8)
                {
                    //Stop motor for at ændre hastighed, 50 til at holde tryk på kasse.
                    std::cout << "Lukke strøm opnået, stopper motor!!!" << std::endl;
                    m.stopMotor();
                    delay(1);
                    m.setSpeed(50);
                    delay(1);
                    m.startMotor();
                    delay(100);

                    //Print størrelse til user
                    std::cout << "Størrelsen på denne kasse er: " << enc.getOrientation() << std::endl;

                    //If statment der holder styr på kassen der lige er blevet grebet. Kasse bliver indsat i databasen alt efter størrelse
                    if(enc.getOrientation() >= 1 && enc.getOrientation() <= 9){
                        //Sæt størrelse på kasse.
                        BoxSizeToRobot47 = 1;
                        //Send størrelse af kassen til robot.
                        my_client.getWriter().sendInputIntRegister(47, BoxSizeToRobot47);
                        //Indsæt værdier i tabellen
                        DB.insertValuesPose("Pose", PoseOnGrip[0], PoseOnGrip[1], PoseOnGrip[2], PoseOnGrip[3], PoseOnGrip[4], PoseOnGrip[5]);
                        DB.insertValuesGreb("Greb", "Lille kasse med størrelsen: " + std::to_string(enc.getOrientation()));
                    }
                    else if(enc.getOrientation() >= 10 && enc.getOrientation() <= 13){
                        BoxSizeToRobot47 = 2;
                        my_client.getWriter().sendInputIntRegister(47, BoxSizeToRobot47);
                        DB.insertValuesPose("Pose", PoseOnGrip[0], PoseOnGrip[1], PoseOnGrip[2], PoseOnGrip[3], PoseOnGrip[4], PoseOnGrip[5]);
                        DB.insertValuesGreb("Greb", "Mellem kasse med størrelsen: " + std::to_string(enc.getOrientation()));
                    }
                    else if(enc.getOrientation() >= 14 && enc.getOrientation() <= 20){
                        BoxSizeToRobot47 = 3;
                        my_client.getWriter().sendInputIntRegister(47, BoxSizeToRobot47);
                        DB.insertValuesPose("Pose", PoseOnGrip[0], PoseOnGrip[1], PoseOnGrip[2], PoseOnGrip[3], PoseOnGrip[4], PoseOnGrip[5]);
                        DB.insertValuesGreb("Greb", "Stor kasse med størrelsen: " + std::to_string(enc.getOrientation()));
                    }
                    else{
                        BoxSizeToRobot47 = 4;
                        my_client.getWriter().sendInputIntRegister(47, BoxSizeToRobot47);
                        DB.insertValuesPose("Pose", PoseOnGrip[0], PoseOnGrip[1], PoseOnGrip[2], PoseOnGrip[3], PoseOnGrip[4], PoseOnGrip[5]);
                        DB.insertValuesGreb("Greb", "Udefineret kasse!");
                    }
                    DB.printTableGreb("Greb");
                    DB.printTablePose("Pose");
                    RP = 20;
                    break;
                }
            }
            delay(1000);
            my_client.getWriter().sendInputBitRegister(125, 1);
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(125, 0);

        break;
    case 20:
        //Når robot er i place position, sæt kasse
        if (RobotInPlacePos126)
        {
            //Stop moment, når robot er i position
            m.stopMotor();
            delay(100);
            //Kør griber helt å
            m.setSpeed(200);
            m.setDirection(0);
            delay(100);
            //Åben griber og sæt kasse.
            m.startMotor();
            delay(1000);
            m.stopMotor();
            RP = 1;
            my_client.getWriter().sendInputBitRegister(126, 1);
            break;

        }
        else
            my_client.getWriter().sendInputBitRegister(126, 0);

        break;
    }
}
  return 0;
}
