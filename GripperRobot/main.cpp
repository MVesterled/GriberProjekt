#include <iostream>
#include <string>
#include <database.h>
#include <motor.h>
#include <memory>
#include <ctime>
#include <thread>
#include <ur_client_library/rtde/rtde_client.h>
#include <Encoder.h>

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

void closeHandler(int sig) {
    exit(0);
}

float get_adc(int channel) {
    if (channel != 0) {
        channel = 1;
    }

    std::vector<unsigned char> data(2);
    data[0] = 0b11;
    data[0] = ((data[0] << 1) + channel) << 5;
    data[1] = 0b00000000;

    wiringPiSPIDataRW(CHANNEL, data.data(), data.size());

    unsigned int adc = 0;
    for (int i = 0; i < 2; ++i) {
        adc = (adc << 8) + data[i];
    }
    adc = adc >> 1;

    float voltage = (5 * adc) / 1024.0;

    return voltage;
}

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

  my_client.init();

  my_client.start();



  static bool RobotInPickPos127 = 0;
  static bool RobotInPlacePos126 = 0;
  static bool RobotReady125 = 0;
  static int spi_fd;
  static float adc_1;
  static bool flag = false;

if ((spi_fd = wiringPiSPISetup(CHANNEL, 1200000)) < 0)
{
      std::cerr << "Failed to initialize SPI." << std::endl;
      return 1;
}

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
        try {
            adc_1 = get_adc(1);
            std::cout << "Spænding over modstand på kanal 1: " << adc_1 << " VDC" << std::endl;
            usleep(25000);
        } catch (...) {
        // Catch all exceptions to ensure cleanup
        std::cerr << "An error occurred." << std::endl;
        }
    }
};

auto getEncoder = [&](){
    while(true){
        enc.updateCounter();
        //std::cout << "Position: " << enc.getOrientation() << std::endl;
    }
};

std::jthread dataThread1(getRobotRTDE);
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
        delay(1000);
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
                enc.setOrientation(0);
                RP = 9;
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
            if(enc.getOrientation() <= -18){
                std::cout << "Nået endestop!" << std::endl;
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
        if (RobotInPickPos127)
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
                    /*
                    delay(30000);
                    std::cout << "Slipper!" << std::endl;
                    m.stopMotor();
                    */

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
