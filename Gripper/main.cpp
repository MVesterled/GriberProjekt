#include <stdlib.h>
#include <iostream>
#include <string>
#include <Database.h>
#include <Motor.h>
//#include <QCoreApplication>
#include <memory>
#include <ctime>
#include <thread>
#include <ur_client_library/rtde/rtde_client.h>
//Ko
using namespace urcl;

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
  // Parse the ip arguments if given
  //std::string robot_ip = DEFAULT_ROBOT_IP;

  // dette skal gøres til member variabler
  /*
  RobComm* rcomm = RobComm::getInstance();
  rcomm->setClientIP()
   */
  comm::INotifier notifier;
  rtde_interface::RTDEClient my_client(DEFAULT_ROBOT_IP, notifier, OUTPUT_RECIPE, INPUT_RECIPE);
  my_client.init();

  // Once RTDE communication is started, we have to make sure to read from the interface buffer, as
  // otherwise we will get pipeline overflows. Therefor, do this directly before starting your main
  // loop.
  my_client.start();


  static bool RobotInPickPos127 = 0;
  static bool RobotInPlacePos126 = 0;
  static bool RobotReady125 = 0;

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

      }
      else
      {
        std::cout << "Could not get fresh data package from robot" << std::endl;
        return 1;
      }

    if (!my_client.getWriter().sendInputBitRegister(127, 1))
    {
      // This will happen for example, when the required keys are not configured inside the input
      // recipe.
      std::cout << "\033[1;31mSending RTDE data failed."
                << "\033[0m\n"
                << std::endl;
      return 1;
    }

    if (RobotReady125)
       my_client.getWriter().sendInputBitRegister(127, 1);
    else
       my_client.getWriter().sendInputBitRegister(127, 0);



    if (RobotInPickPos127)
       my_client.getWriter().sendInputBitRegister(125, 1);
    else
       my_client.getWriter().sendInputBitRegister(125, 0);



    if (RobotInPlacePos126)
       my_client.getWriter().sendInputBitRegister(126, 1);
    else
       my_client.getWriter().sendInputBitRegister(126, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

   }

  return 0;
}









/*
int main(void) {
    
    
    
        wiringPiSetupGpio(); //initializer for gpio
        Motor m;
        /*
        std::cout << "Set speed: (max 512)" << std::endl;
        int tmp=0;
        std::cin >> tmp;
        m.setSpeed(tmp);
        bool dir;
        //h
        while (true){
        std::cout << "Set direction: (1 = right) (0 = left)" << std::endl;
        std::cin >> dir;
        m.setDirection(dir);
        m.startMotor();
        delay(3000);
        m.stopMotor();
}   
        
        
        

/*
    std::cout << "Program startet" << std::endl;
    std::cout << std::endl;
    Database griberData;
    //Motor testObject2;
    //testObject2.printTest2(); 
    std::string db = "test_db";
    std::string table = "force";
    
    std::cout << std::endl;
    griberData.useDatabase(db);
    std::cout << std::endl;
    griberData.createTable(table);
    std::cout << std::endl;
    griberData.insertValues(1, table, "25 Newton");
    std::cout << std::endl;
    griberData.printTable(table);
    std::cout << std::endl;
    griberData.deletePointers();
    std::cout << std::endl;
    std::cout << "Program færdigt" << std::endl;
    



    return 0; // Return success
}

*/

