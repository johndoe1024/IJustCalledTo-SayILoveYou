#pragma once

namespace ip {

class IUserInterface {
  public:
   virtual ~IUserInterface() {}
   virtual void Run() = 0;
   //virtual void Exit() = 0;
};

}  // namespace imp
