<?php
namespace Model;

class Team extends \Model {
  public static $roles = array(
    0 => array(
      "name" => "Cardinal",
      "permission" => array(
        "cardinal" => true
      )
    ),
    1 => array(
      "name" => "Administrator",
      "permission" => array(
        "edit_profile" => true,
        "edit_leagues" => true,
        "edit_team" => true,
        "dashboard" => true
      ),
      "available" => true
    ),
    2 => array(
      "name" => "Editor",
      "permission" => array(
        "edit_leagues" => true,
        "dashboard" => true
      ),
      "available" => true
    ),
    3 => array(
      "name" => "User",
      "permission" => array(
      )
    ),
  );

  public static function role($id) { return static::$roles[$id]; }


}
