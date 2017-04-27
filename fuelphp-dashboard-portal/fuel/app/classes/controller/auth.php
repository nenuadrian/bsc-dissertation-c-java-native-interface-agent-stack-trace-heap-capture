<?php
use \Model\API;
use \Model\Auth;
use \Model\APIUser;

class Controller_Auth extends Controller {

	public function action_user($token, $redirect = false) {
		$token = base64_decode($token);
		if ($redirect) $redirect = base64_decode($redirect);
		\Session::set('token', $token);
		$user = APIUser::get('in/user');
		Auth::start_session($token, $user['data']);
		if ($redirect)
			Response::redirect(Uri::create($redirect));
		else
			Response::redirect(Uri::base());
	}

	public function action_index() {
		if (Auth::check()) Response::redirect(Uri::base());
      if (Input::post()) {
					$user = API::authenticate(Input::param('username'), hash('sha256', Input::param('password')));
					if ($user) {
						Messages::success('Welcome!');
						Auth::start_session($user['token'], $user['user']);
						Response::redirect_back('dashboard');
					} else {
						  Messages::error("Access denied!");
					}
      }

      return View::forge('auth/login');
  }

	public function action_register() {
		if (Auth::check()) Response::redirect(Uri::base());
      if (Input::post()) {
					$user = API::register(Input::param('username'), Input::param('email'), hash('sha256', Input::param('password')));
					if ($user) {
						Messages::success('Welcome!');
						Auth::start_session($user['token'], $user['user']);
						Response::redirect_back('dashboard');
					} else {
						  Messages::error("Access denied!");
					}
      }

      return View::forge('auth/register');
  }


  public function action_logout() {
		Auth::logout();
    Messages::success('Logged out');
    Response::redirect_back();
  }
}
