<?php

namespace Model;

class API extends \Model {
  public static $extraEndpoint = '';

  public static function sponsor() { return Auth::sponsor() ? 'in/sponsors/' . Auth::sponsor() . '/' : 'in/'; }

  public static function authenticate($username, $password) {
    $result = static::post('auth/authenticate', array('username' => $username, 'password' => $password));
    if ($result['done']) {
      return $result['data'];
    }
    return false;
  }

  public static function register($username, $email, $password) {
    $result = static::post('auth/signup', array('username' => $username, 'email' => $email, 'password' => $password));
    if ($result['done']) {
      return $result['data'];
    }
    return false;
  }

  public static function post($endpoint, $data = array()) {
    return static::server($endpoint, \Requests::POST, $data);
  }

  public static function get($endpoint) {
    return static::server($endpoint, \Requests::GET, array());
  }

  public static function put($endpoint, $data = array()) {
    return static::server($endpoint, \Requests::PUT, $data);
  }

  public static function delete($endpoint, $data = array()) {
    return static::server($endpoint, \Requests::DELETE, $data);
  }

  public static function server($endpoint, $method, $data) {
    try {
      $endpoint = \Config::get('api-server') . static::$extraEndpoint . $endpoint;
      $headers = array(
        'Content-Type' => 'application/json',
        'Accept' => 'application/json'
      );
      if (\Session::get('token'))
        $headers['auth-token'] = \Session::get('token');

      if ($method == \Requests::GET) {
         $request = \Requests::request($endpoint, $headers, $data, $method);
      } elseif ($data) {
        $request = \Requests::request($endpoint, $headers, json_encode($data), $method);
      } else $request = \Requests::request($endpoint, $headers, null, $method);
      $response = json_decode($request->body, true);

      if ($response === null) return array('done' => false, 'error' => 'empty');
      if (!$response['done'] && isset($response['message']) && $response['message'] == 'invalid_session') {
        \Session::delete('token');
        \Response::redirect(\Uri::create('auth'));
      }
      return $response;
    } catch (\Exception $ex){
      \Messages::error("An error took place while communicating with our servers. If this persists, please contact our team! <div>" . base64_encode($ex->getMessage()) . "</div>");
      \Response::redirect(\Uri::base());
    }

  }
}
