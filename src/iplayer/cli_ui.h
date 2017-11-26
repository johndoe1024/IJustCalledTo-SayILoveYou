#pragma once

#include "iplayer/i_user_interface.h"

#include <memory>
#include <thread>

#include "iplayer/i_player_control.h"

namespace ip {

class Cli : public IUserInterface {
 public:
  Cli(std::unique_ptr<IPlayerControl> player_ctl);
  virtual ~Cli();

  void Run() override;
  void Exit() override;

 private:
  void Dispatch(const std::string& command, const std::string& parameters);
  void UiThread();

  std::thread ui_thread_;
  std::unique_ptr<IPlayerControl> player_ctl_;
};

}  // namespace ip