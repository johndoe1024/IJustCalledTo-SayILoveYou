#pragma once

#include "iplayer/i_user_interface.h"

#include <memory>

#include "iplayer/i_player_control.h"

namespace ip {

class Cli : public IUserInterface {
  public:
    Cli(std::unique_ptr<IPlayerControl> player_ctl);

    void Run() override;
    //void Exit() override;

  private:
    std::unique_ptr<IPlayerControl> player_ctl_;
    void Dispatch(const std::string& command, const std::string& parameters);
};

}  // namespace imp
