#include <iostream>
#include <string>
#include <database.h>
#include <motor.h>
#include <memory>
#include <ctime>
#include <thread>
#include <ur_client_library/rtde/rtde_client.h>
#include <Encoder.h>
#include "AmMeter.h"

#include <wiringPiSPI.h>
using namespace urcl;

#define CHANNEL 0

// In a real-world example it would be better to get those values from command line parameters / a better configuration
// system such as Boost.Program_options
const std::string DEFAULT_ROBOT_IP = "192.168.1.54";
const std::string OUTPUT_RECIPE = "/home/pi/RobCom/griber/Desktop/SemesterProjekt/SemesterProgram/rtde_output_recipe.txt";
const std::string INPUT_RECIPE = "/home/pi/RobCom/griber/Desktop/SemesterProjekt/SemesterProgram/rtde_input_recipe.txt";
const std::chrono::milliseconds READ_TIMEOUT{ 1000 };

// Static member for RobComm
//RobComm*  RobComm::mRComm = nullptr;

int main(int argc, char* argv[])
{
  // dette skal gøres til member variabler
  //RobComm* rcomm = RobComm::getInstance();
  //rcomm->setClientIP()

  wiringPiSetupGpio(); //initializer for gpio

  comm::INotifier notifier;
  rtde_interface::RTDEClient my_client(DEFAULT_ROBOT_IP, notifier, OUTPUT_RECIPE, INPUT_RECIPE);
  Encoder enc;
  Motor m;
  AmMeter AM;
  Database DB;
  DB.createDatabase("GriberData");
  DB.useDatabase("GriberData");
  DB.createTable("Greb");


/*
  my_client.init();
  my_client.start();
*/


  static bool RobotInPickPos127 = 0;
  static bool RobotInPlacePos126 = 0;
  static bool RobotReady125 = 0;
  static float adc_1;
  static bool flag = false;
  static int tempEnc = 0;



auto getRobotRTDE = [&](){
    while (true)
    {
      std::unique_ptr<rtde_interface::DataPackage> data_pkg = my_client.getDataPackage(READ_TIMEOUT);
      if (data_pkg)
      {
        //std::cout << data_pkg->toString() << std::endl;

        data_pkg->getData("output_bit_register_127", RobotInPickPos127);
        data_pkg->getData("output_bit_register_126", RobotInPlacePos126);
        data_pkg->getData("output_bit_register_125", RobotReady125);
/*
        std::cout << "RobInPickPos127: " << RobotInPickPos127 << std::endl;
        std::cout << "RobInPickPos126: " << RobotInPlacePos126 << std::endl;
        std::cout << "RobotReady125: " << RobotReady125 << std::endl;
        */
      }

      else
      {
        std::cout << "Could not get fresh data package from robot" << std::endl;
        return 1;
      }
    }
};

auto getVoltage = [&](){
    while(true){
        if(!flag){
            try {
                adc_1 = AM.getADC(1);
                std::cout << "Spænding over modstand på kanal 1: " << adc_1 << " VDC" << std::endl;
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

//std::jthread dataThread1(getRobotRTDE);
std::jthread dataThread2(getVoltage);
std::jthread dataThread3(getEncoder);

int RP = 1;

while(true){
    // switch case
    switch (RP) {

    case 1:
        //zero robot at center
        m.setSpeed(200);
        m.setDirection(0);
        delay(100);
        m.startMotor();
        delay(500);
        m.stopMotor();
        delay(1000);
        m.setDirection(1);
        delay(100);
        m.startMotor();
        delay(600);
        while(true){
            if(adc_1 > 4.6)
            {
                std::cout << "Center nået, nulstil encoder!" << std::endl;
                m.stopMotor();
                delay(500);
                m.setSpeed(50);
                m.setDirection(0);
                delay(100);
                m.startMotor();
                delay(300);
                m.stopMotor();
                delay(100);
                enc.setOrientation(0);
                RP = 9;
                std::cout << enc.getOrientation() << std::endl;
                break;
            }
        }

        break;
    case 9:
        //Move robot to full open
        m.setSpeed(200);
        m.setDirection(0);
        delay(1000);
        m.startMotor();
        std::cout << "Køre mod endestop!" << std::endl;
        while(true){
            if(enc.getOrientation() >= 19){
                std::cout << "Nået endestop!" << std::endl;
                m.stopMotor();
                delay(1);
                m.setSpeed(100);
                m.setDirection(1);
                delay(10);
                m.startMotor();
                delay(100);
                m.stopMotor();
                RP = 15;
                break;
            }
        }
        break;

    case 10:
        // if robot ready, send start command (true)
        if (RobotReady125) {
            my_client.getWriter().sendInputBitRegister(127, 1);
            RP = 1;
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(127, 0);

        break;
    case 15:
        //If robot in pick pos, run motor for gripper close with ADC
        if (RobotInPickPos127 || true)
        {
            //Kør mod luk!
            m.setSpeed(200);
            m.setDirection(1);
            delay(500);
            std::cout << "Start motor luk!" << std::endl;
            m.startMotor();
            delay(600);

            while(true){
                if(adc_1 > 4.6) // kør indtil måling er høj
                {
                    flag = true;
                    std::cout << "Lukke strøm opnået, stopper motor!!!" << std::endl;
                    m.stopMotor();
                    delay(1);
                    m.setSpeed(50);
                    delay(1);
                    m.startMotor(); // start moment og hold mens robot bevæger sig
                    delay(100);
                    std::cout << "Størrelsen på denne kasse er: " << enc.getOrientation() << std::endl;

                    //If statment der holder styr på kassen der lige er blevet grebet. Kasse bliver indsat i databasen alt efter størrelse
                    if(enc.getOrientation() >= 6 && enc.getOrientation() <= 9){

                        DB.insertValues(1, "Greb", "Lille kasse med størrelsen: " + std::to_string(enc.getOrientation()));

                    }
                    else if(enc.getOrientation() >= 11 && enc.getOrientation() <= 13){

                        DB.insertValues(2, "Greb", "Mellem kasse med størrelsen: " + std::to_string(enc.getOrientation()));

                    }
                    else if(enc.getOrientation() >= 15 && enc.getOrientation() <= 18){

                        DB.insertValues(3, "Greb", "Stor kasse med størrelsen: " + std::to_string(enc.getOrientation()));

                    }
                    else{

                        DB.insertValues(10, "Greb", "Udefineret kasse!");

                    }
                    DB.printTable("Greb");
                    flag = true;
                    delay(5000);
                    std::cout << "Slipper!" << std::endl;
                    m.stopMotor();

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
        // if robot in place pos, run motor for gripper open
        if (RobotInPlacePos126)
        {

            m.stopMotor(); // stop moment, når robot er i position
            delay(100);

            //Kør mod åben!
            m.setSpeed(200);
            m.setDirection(1);
            delay(100);
            m.startMotor();
            while(true){
                if(enc.getOrientation() <= -22){
                    m.stopMotor();
                    delay(1);
                    m.setSpeed(0);
                    delay(1);
                    m.startMotor();
                    delay(100);
                    m.stopMotor();
                    RP = 10;
                    break;
                }
            }

            my_client.getWriter().sendInputBitRegister(126, 1);
        }
        else
            my_client.getWriter().sendInputBitRegister(126, 0);

        break;
    }
}
  return 0;
}
