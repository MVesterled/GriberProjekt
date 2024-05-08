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

if ((spi_fd = wiringPiSPISetup(CHANNEL, 1200000)) < 0)
{
      std::cerr << "Failed to initialize SPI." << std::endl;
      return 1;
}

auto dataFunction = [&](){
    while (true)
      {
          std::unique_ptr<rtde_interface::DataPackage> data_pkg = my_client.getDataPackage(READ_TIMEOUT);
          if (data_pkg)
          {
            std::cout << data_pkg->toString() << std::endl;

            std::string MainData = data_pkg->toString();

            data_pkg->getData("output_bit_register_127", RobotInPickPos127);
            data_pkg->getData("output_bit_register_126", RobotInPlacePos126);
            data_pkg->getData("output_bit_register_125", RobotReady125);

            std::cout << "RobInPickPos127: " << RobotInPickPos127 << std::endl;
            std::cout << "RobInPickPos126: " << RobotInPlacePos126 << std::endl;
            std::cout << "RobotReady125: " << RobotReady125 << std::endl;

            enc.updateCounter();
            std::cout << "Position: " << enc.getOrientation() << std::endl;
         }

          else
          {
            std::cout << "Could not get fresh data package from robot" << std::endl;
            return 1;
          }

      try {

              float adc_1 = get_adc(1);
              std::cout << "V ADC Channel 1: " << adc_1 << "V" << std::endl;

      } catch (...)
        {
          // Catch all exceptions to ensure cleanup
          std::cerr << "An error occurred." << std::endl;
        }
    }
};



std::jthread dataThread(dataFunction);

int RP = 0;

while(true){
    // switch case
    switch (RP) {
    case 0:
        // if robot ready, send start command (true)
        if (RobotReady125) {
            my_client.getWriter().sendInputBitRegister(127, 1);
            RP = 1;
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(127, 0);

        break;
    case 1:
        //If robot in pick pos, run motor for gripper close
        if (RobotInPickPos127)
        {
            //Kør mod luk!
            m.setSpeed(400);
            m.setDirection(0);
            delay(1000);
            m.startMotor();
            delay(2000);
            m.stopMotor();
            delay(1000);
            my_client.getWriter().sendInputBitRegister(125, 1);
            RP = 2;
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(125, 0);

        break;
    case 2:
        // if robot in place pos, run motor for gripper open
        if (RobotInPlacePos126)
        {
            //Kør mod åben!
            m.setSpeed(400);
            m.setDirection(1);
            delay(1000);
            m.startMotor();
            delay(2000);
            m.stopMotor();
            delay(1000);
            my_client.getWriter().sendInputBitRegister(126, 1);
            RP = 0;
            break;
        }
        else
            my_client.getWriter().sendInputBitRegister(126, 0);

        break;
    }
}
  return 0;
}
