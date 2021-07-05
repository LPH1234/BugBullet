#pragma once
#include<string>



const std::string WINDOW_TITLE = "Bug子弹";
const std::string WINDOW_TITLE_EN = "B U G   B U L L E T";
const std::string ICON_PATH = "images/icon/icon.png";


const std::string COOKING_DIR = "cooking/";
const std::string COOKING_FILE_SUFFIX = ".cooking";
const std::string COOKING_CONVEX_PREFIX = "c_";
const std::string COOKING_TRIANGLE_PREFIX = "t_";

const std::string ACTOR_NAME_PLAYER_BULLET = "bullet";

const std::string HETP_TEXT = "\
Press The W,S,A,D,Q,E Key To Control The Movement And \n\
Orientation of Fighter, Press Space Key To Emit Missile.\n\
";

const unsigned int MAX_GAME_LEVEL = 3;
const std::string GAME_MISSION_TITLE = "M I S S I O N     T A R G E T";
const std::string GAME_MISSION_ICON = "images/textures/goal.png";
const std::string GAME_MISSION_TANK_TEXT = "Tanks On East Island(%d/%d)";
const std::string GAME_MISSION_TANK_ICON = "images/textures/tank.png";
const std::string GAME_MISSION_TOWER_TEXT = "Anti-Aircraft On North Island(%d/%d)";
const std::string GAME_MISSION_TOWER_ICON = "images/textures/tower.png";
const std::string GAME_MISSION_PLANE_TEXT = "Hostile Aircraft In The Air(%d/%d)";
const std::string GAME_MISSION_PLANE_ICON = "images/textures/plane.png";
const std::string GAME_MISSION_COMPLETE_ICON = "images/textures/check.png";
const std::string GAME_MISSION_DISCOMPLETE_ICON = "images/textures/check-unactive.png";

const float EN_LOGO_FONT_SIZE = 100;
const std::string EN_FONT_PATH = "resources/fonts/troika.ttf";
const float EN_FONT_SIZE = 20;
const std::string EN_TITLE_FONT_PATH = "resources/fonts/cunia.ttf";
const float EN_TITLE_FONT_SIZE = 26;
const std::string ZH_CN_FONT_PATH = "resources/fonts/zh-cn-1.ttf";
const float ZH_CN_FONT_SIZE = 26;

const unsigned int CENTER_TEXT_HEIGHT = 100U;
const unsigned int CENTER_TEXT_Y_OFFSET = 100U;
const unsigned int CENTER_TEXT_MAX_BLING_TIMES = 5;
const float CENTER_TEXT_BLING_VELOCITY = 0.02f;

const float BORDER_MASK_UI_CLOSE_DELAY = 5;
const float BORDER_MASK_UI_BLING_VELOCITY = 0.02;
const std::string BORDER_MASK_UI_TEX_PATH = "images/textures/borderMask.png";

const std::string CUBE_TEXTURE_PATH = "images/textures/w200Bullet.png";
const std::string BLOOD_TEXTURE_PATH = "images/textures/blood.png";


const std::string GRAY_CLOUD_TEXTURE_PATH = "images/textures/smoke/gray-cloud.png";
const std::string WHITE_CLOUD_TEXTURE_PATH = "images/textures/smoke/white-cloud.png";
const std::string GRAY_SMOKE_TEXTURE_PATH = "images/textures/smoke/gray-smoke.png";


const std::string PAUSE_ICON_PATH = "images/textures/pause.png";

const std::string HPBAR_TEXTURE_PATH = "images/textures/hpbar-border.png";

const std::string RETICLE_TEXTURE_PATH = "images/textures/target.png";
const float RETICLE_SIZE_RELATIVE_FACTOR = 15.f;
const float RETICLE_BEGIN_TO_BLING_DISTANCE = 300.f;
const float RETICLE_LOCK_DISTANCE = 15.f;
const float RETICLE_BLING_VELOCITY = 0.1f;

const unsigned int ANIMATE_START_FRAME = 946; //主界面动画开始帧
const  std::string ANIMATE_FRAMS_PATH = "resources/animation/BattleField2042/images/";
const  std::string ANIMATE_FRAMS_SUFFIX = ".png";

const std::string OVER_ICON_PATH = "images/textures/fail.png";
const std::string OVER_TITLE_TEXT = "M I S S I O N     F A I L";
const std::string OVER_CONTENT_TEXT = "YOU CRASHED.";
const std::string OVER_BUTTON1_TEXT = "BACK TO MAIN";

const std::string CONNER_TIP_POSITION_ICON = "images/textures/position.png";
const std::string CONNER_TIP_POSITION_TEXT = "Position: (%.1f, %.1f, %.1f)";
const std::string CONNER_TIP_ORIENTATION_ICON = "images/textures/orientation.png";
const std::string CONNER_TIP_ORIENTATION_TEXT = "Orientation: (%.1f, %.1f, %.1f)";
const std::string CONNER_TIP_FPS_TEXT = "Application average %.2f ms/frame (%.1f FPS)";
const std::string CONNER_TIP_FPS_ICON = "images/textures/fps.png";

const std::string PLAYER_STATUS_HP_ICON = "images/textures/heart.png";
const std::string PLAYER_STATUS_HP_TEXT = "HP";
const std::string PLAYER_STATUS_AMMO_ICON = "images/textures/ammo.png";
const std::string PLAYER_STATUS_AMMO_TEXT = "Ammo";
const std::string PLAYER_STATUS_MISSILE_ICON = "images/textures/missile.png";
const std::string PLAYER_STATUS_MISSILE_TEXT = "Missile";

const unsigned int PLAYER_MIN_HP_TO_ALERT = 30;