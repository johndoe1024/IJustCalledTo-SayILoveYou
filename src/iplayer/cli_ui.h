#pragma once

#include "iplayer/i_user_interface.h"

#include <future>
#include <memory>

#include "iplayer/i_player_control.h"

namespace ip {

class Cli : public IUserInterface {
 public:
  Cli(std::unique_ptr<IPlayerControl> player_ctl);
  virtual ~Cli();

  void Run() override;  // start ui thread
  void Exit() override;  // set ui thread to stop and block until done

 private:
  void Dispatch(const std::string& command, const std::string& parameters);
  void UiThread();

  std::future<void> cli_future_;
  std::unique_ptr<IPlayerControl> player_ctl_;
};

}  // namespace ip
