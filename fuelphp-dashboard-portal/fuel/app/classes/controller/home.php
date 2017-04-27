<?php
use \Model\Auth;

class Controller_Home extends Controller {
	public function action_index() {
		return View::forge('home/index');
	}

	public function action_404() {
		return View::forge('home/404');
	}
}
