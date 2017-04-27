<?php
use \Model\API;
use \Model\Azure;
use \Model\Auth;
use \Model\APIUser;

class Controller_User extends Controller {
	public function __construct() { }

  public function action_reward($reward_id) {
    $tVars = array();
    $reward = APIUser::get('in/rewards/' . $reward_id)['data'];

    $tVars['reward'] = $reward;
    return View::forge('user/user_reward', $tVars);
  }
}
