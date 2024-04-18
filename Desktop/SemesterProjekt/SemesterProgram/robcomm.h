#ifndef ROBCOMM_H
#define ROBCOMM_H

#include <QCoreApplication>
#include <ur_client_library/rtde/rtde_client.h>
#include <iostream>
#include <memory>
#include <ctime>
#include <string>

using namespace urcl;

class RobComm
{
public:
    RobComm(const RobComm& r) = delete;

    void operator=(const RobComm&) = delete;

    static RobComm* getInstance() { if (mRComm == nullptr) mRComm = new RobComm; return mRComm; }

    void setClientIP(std::string IP);
    void setInputRecipe(std::string InputRecipe);
    void setOutputRecipe(std::string OutputRecipe);
    void setNotifier(comm::INotifier Notifier);

    std::string getClientIP() const;
    std::string getInputRecipe() const;
    std::string getOutputRecipe() const;
    comm::INotifier getNotifier() const;

private:
   RobComm();

   static RobComm* mRComm;

   std::string mIP;


   const std::string mDEFAULT_ROBOT_IP = "192.168.1.54";
   const std::string mOUTPUT_RECIPE = "/home/mmwar/Desktop/SemesterProjekt/SemesterProgram/rtde_output_recipe.txt";
   const std::string mINPUT_RECIPE = "/home/mmwar/Desktop/SemesterProjekt/SemesterProgram/rtde_input_recipe.txt";
   const std::chrono::milliseconds mREAD_TIMEOUT{ 1000 };

   comm::INotifier mNotifier;
   rtde_interface::RTDEClient mMy_client;
};

#endif // ROBCOMM_H
