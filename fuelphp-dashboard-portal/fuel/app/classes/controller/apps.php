<?php
use \Model\API;
use \Model\Auth;

class Controller_Apps extends Controller {
  public function __construct() { if (!Auth::check()) Response::redirect(Uri::base()); }
  public function action_index() {
    if (Input::post('action') == "add") {
      $result = API::post('in/apps');
      if ($result['done']) {
        Response::redirect(Uri::current());
      }
    }
    $apps = API::get('in/apps')['data'];
    $tVars = array();
    $tVars['apps'] = $apps;
		return View::forge('apps/apps_index', $tVars);
	}

  public function action_log($app_id, $id, $st = false) {
    $log = API::get($endpoint = 'in/apps/' . $app_id . '/logs/' . $id)['data'];
    $tVars = array();
    $log['message'] = Format::forge($log['message'], 'xml')->to_array();
    $tVars['log'] = $log;
    $tVars['app_id'] = $app_id;
    $tVars['selected_st'] = $st === false ? 0 : $st;
    return View::forge('apps/apps_log', $tVars);
  }

  public function action_app($id) {
    $app = API::get($endpoint = 'in/apps/' . $id)['data'];

    if (Input::post('action') == 'update') {
      $app = API::post($endpoint = 'in/apps/' . $id, Input::post())['data'];
      Response::redirect(Uri::current());
    }

    $logs = API::get($endpoint . '/logs')['data'];
    foreach ($logs as $k => $log) {
      if ($log['log_type'] != 'exception') {
        unset($logs[$k]);
      }
      //print_R(Format::forge($log['message'], 'xml'));
    }
    $tVars = array();
    $tVars['app'] = $app;
    $tVars['logs'] = $logs;
		return View::forge('apps/apps_app', $tVars);
	}

}
