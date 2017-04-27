<?php
namespace Model;

class Auth extends \Model {

  public static function start_session($token, $data) {
    \Session::set('token', $token);
    if (!isset($data['type'])) {
      $data['type'] = 3;
    }
    \Session::set('user', $data);
  }

  public static function role() { return static::get('type'); }
  public static function get($field = false) { return $field ? \Session::get('user')[$field] : \Session::get('user'); }
  public static function set($user) { \Session::set('user', $user); }
  public static function can($perm) { return static::check() && (isset(Team::$roles[static::role()]['permission']['cardinal']) || isset(Team::$roles[static::role()]['permission'][$perm])); }
  public static function check() { return \Session::get('token') ? true : false; }
  public static function logout() { \Session::destroy(); }

}
