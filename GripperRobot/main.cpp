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

  comm::INotifier notifier;
  rtde_interface::RTDEClient my_client(DEFAULT_ROBOT_IP, notifier, OUTPUT_RECIPE, INPUT_RECIPE);
  Encoder enc;

  wiringPiSetupGpio(); //initializer for gpio
  Motor m;

  my_client.init();

  my_client.start();


  static bool RobotInPickPos127 = 0;
  static bool RobotInPlacePos126 = 0;
  static bool RobotReady125 = 0;
  static int spi_fd;
  static bool falg1, flag2, flag3;

  while (true)
  {
    // Read latest RTDE package. This will block for READ_TIMEOUT, so the
    // robot will effectively be in charge of setting the frequency of this loop unless RTDE
    // communication doesn't work  in which case the user will be notified.
    // In a real-world application this thread should be scheduled with real-time priority in order
    // to ensure that this is called in time.
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

      if ((spi_fd = wiringPiSPISetup(CHANNEL, 1200000)) < 0) {
          std::cerr << "Failed to initialize SPI." << std::endl;
          return 1;
      }

      try {

              float adc_0 = get_adc(0);
              float adc_1 = get_adc(1);
              std::cout << "V ADC Channel 1: " << adc_1 << "V" << std::endl;

      } catch (...) {
          // Catch all exceptions to ensure cleanup
          std::cerr << "An error occurred." << std::endl;
      }

  /*
    if (!my_client.getWriter().sendInputBitRegister(127, 1))
    {
      // This will happen for example, when the required keys are not configured inside the input
      // recipe.
      std::cout << "\033[1;31mSending RTDE data failed."
                << "\033[0m\n"
                << std::endl;
      return 1;
    }
*/
    if (RobotReady125)
       my_client.getWriter().sendInputBitRegister(127, 1);
    else
       my_client.getWriter().sendInputBitRegister(127, 0);


//If robot in pick pos, run motor for gripper close
    if (RobotInPickPos127) {
        //Kør mod luk!
        m.setSpeed(175);
        m.setDirection(1);
        delay(1000);
        m.startMotor();
        delay(800);
        m.stopMotor();
        delay(2000);
        my_client.getWriter().sendInputBitRegister(125, 1);
    }
    else
       my_client.getWriter().sendInputBitRegister(125, 0);



    if (RobotInPlacePos126) {
       //Kør mod åben!
       m.setSpeed(175);
       m.setDirection(0);
       delay(1000);
       m.startMotor();
       delay(800);
       m.stopMotor();
       delay(2000);
       my_client.getWriter().sendInputBitRegister(126, 1);
    }
    else
       my_client.getWriter().sendInputBitRegister(126, 0);

   }
  return 0;
}
