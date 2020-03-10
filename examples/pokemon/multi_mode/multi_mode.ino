/**
* 育て屋から卵を回収→孵化→ボックスに預けるを繰り返すスケッチ
* ボックスに空きがある限り、ポケモンを孵化し続ける
*
* 初期条件は以下の通り
* 1.ハシノマはらっぱにいること
* 2.自転車に乗っていること
* 3.手持ちが1体のみのこと
* 4.Xボタンを押したときに「タウンマップ」が左上、「ポケモン」がその右にあること
* 5.ボックスが空のこと
*/
#include <auto_command_util.h>

const int TIME_TO_HATCHING_SEC = 135;

// Buttonに使うピン
const uint16_t BTN_VCC_PIN = 10;
const uint16_t BTN_IN1_PIN = 7;
const uint16_t BTN_IN2_PIN = 11;

// Speakerに使うピン
const uint16_t SPK_GND_PIN = 12;
const uint16_t SPK_IN_PIN = 9;

// mode
const uint16_t MODE_BITS = 4;
int mode = 0;

// レイドバトルが終わるまでの時間
const int BATTLE_FINISH_SEC = 200;

// 日付変更用
int day_count = 1;

// その他，キャッシュ用変数
int v0 = 0, v1 = 0, v2 = 0, v3 = 0;

// 空飛ぶタクシーでハシノマはらっぱに移動
void moveToInitialPlayerPosition()
{
  pushButton(Button::X, 1000);
  pushHatButtonContinuous(Hat::LEFT_UP, 1000);
  pushButton(Button::A, 2400);
  pushButton(Button::A, 1300, 2);
  delay(2000);
}

// 初期位置から育て屋さんに移動し卵を受け取る
void getEggFromBreeder()
{
  // 初期位置(ハシノマはらっぱ)から育て屋さんのところまで移動
  pushButton(Button::PLUS, 1000);
  tiltJoystick(0, 0, 100, 0, 2000);
  tiltJoystick(30, -100, 0, 0, 2000);
  pushButton(Button::PLUS, 1000);
  // 育て屋さんから卵をもらう
  pushButton(Button::A, 1000, 4);
  pushButton(Button::B, 500, 10);
}

// 初期位置(ハシノマはらっぱ)からぐるぐる走り回る
void runAround(int run_time_sec)
{
  // 野生ポケモンとのエンカウントを避けるため初期位置から少し移動する
  tiltJoystick(100, 0, 0, 0, 600);
  // delayの秒数がintの最大値を越えないように30秒ごとに実行する
  for (int i = 0; i < run_time_sec / 30; i++)
  {
    tiltJoystick(100, 100, -100, -100, 30000);
  }
  tiltJoystick(100, 100, -100, -100, (run_time_sec % 30) * 1000);
}

// 卵が孵化するのを待つ
void waitEggHatching()
{
  pushButton(Button::B, 500, 40);
}

// 孵化した手持ちのポケモンをボックスに預ける
// box_line : 何列目にポケモンを預けるか
void sendHatchedPoemonToBox(int box_line)
{
  // ボックスを開く
  pushButton(Button::X, 1000);
  pushHatButtonContinuous(Hat::LEFT_UP, 1000);
  pushHatButton(Hat::RIGHT, 500);
  pushButton(Button::A, 2000);
  pushButton(Button::R, 2000);
  // 手持ちの孵化したポケモンを範囲選択
  pushHatButton(Hat::LEFT, 500);
  pushHatButton(Hat::DOWN, 500);
  pushButton(Button::Y, 500, 2);
  pushButton(Button::A, 500);
  pushHatButtonContinuous(Hat::DOWN, 2000);
  pushButton(Button::A, 500);
  // ボックスに移動させる
  pushHatButton(Hat::RIGHT, 500, box_line + 1);
  pushHatButton(Hat::UP, 500);
  pushButton(Button::A, 500);
  // ボックスを閉じる
  pushButton(Button::B, 1500, 3);
}

// ボックスを次のボックスに移動させる
void moveToNextBox()
{
  // ボックスを開く
  pushButton(Button::X, 1000);
  pushHatButtonContinuous(Hat::LEFT_UP, 1000);
  pushHatButton(Hat::RIGHT, 500);
  pushButton(Button::A, 2000);
  pushButton(Button::R, 2000);
  // ボックスを移動
  pushHatButton(Hat::UP, 500);
  pushHatButton(Hat::RIGHT, 500);
  // ボックスを閉じる
  pushButton(Button::B, 1500, 3);
}

// 手持ちが1体の状態から、卵受け取り→孵化を繰り返す
void reciveAndHatchEggs()
{
  for (int egg_num = 0; egg_num < 5; egg_num++)
  {
    moveToInitialPlayerPosition();
    getEggFromBreeder();
    moveToInitialPlayerPosition();
    runAround(TIME_TO_HATCHING_SEC);
    waitEggHatching();
  }
}

// 孵化シーケンスを実行
void execHatchingSequence()
{
  for (int box_line = 0; box_line < 6; box_line++)
  {
    reciveAndHatchEggs();
    sendHatchedPoemonToBox(box_line);
  }
  moveToNextBox();
}

// 巣穴の前からひとりレイドを始め、レイドポケモンを倒し、捕まえる
void startRaidBattle(){
    // ひとりではじめる
    pushButton(Button::A, 1500);
    pushHatButton(Hat::DOWN, 500);
    // レイドバトル中はA連打
    for(int i=0; i<BATTLE_FINISH_SEC; i++){
        pushButton(Button::A, 500, 2);
    }
}

// レイドバトル後もしばらく続くAボタン連打の後の画面から、
// 巣穴の前の最初のポジションに戻す
void resetStateRaidBattle(){
    tiltJoystick(0,0,0,0,100);
    pushButton(Button::B, 1000, 4);
    delay(1000);
    pushButton(Button::A, 1000, 2);
    pushButton(Button::B, 1000, 3);
}

// 捕獲した手持ちのポケモンをボックスに預ける
// box_line : 何列目にポケモンを預けるか
void sendCapturePokemonToBox(int box_line)
{
    // ボックスを開く
    pushButton(Button::X, 1000);
    pushHatButtonContinuous(Hat::LEFT_UP, 1000);
    pushHatButton(Hat::RIGHT, 500);
    pushButton(Button::A, 2000);
    pushButton(Button::R, 2000);
    // 手持ちの捕獲したポケモンを範囲選択
    pushHatButton(Hat::LEFT, 500);
    pushHatButton(Hat::DOWN, 500);
    pushButton(Button::Y, 500, 2);
    pushButton(Button::A, 500);
    pushHatButtonContinuous(Hat::DOWN, 2000);
    pushButton(Button::A, 500);
    // ボックスに移動させる
    pushHatButton(Hat::RIGHT, 500, box_line+1);
    pushHatButton(Hat::UP, 500);
    pushButton(Button::A, 500);
    // ボックスを閉じる
    pushButton(Button::B, 1500, 3);
}

// レイドバトルをし、ポケモンを捕まえ、ボックスに預けるを繰り返し、
// ボックスが一杯になったら次のボックスへ移動する
void execRaidBattleSequence(){
    for(int send_line=0; send_line<6; send_line++){
        // 手持ちが一杯になるまでレイドバトルを行う
        for(int i=0; i<5; i++){
            startRaidBattle();
            resetStateRaidBattle();
        }
        // 捕まえたポケモンを全て預ける
        sendCapturePokemonToBox(send_line);
    }
    // ボックスが一杯になったところでボックスを移動する
    moveToNextBox();
}

void goToTimeSetting()
{
  // ホーム画面 > 設定
  pushButton(Button::HOME, 500);
  pushHatButton(Hat::DOWN, 30);
  pushHatButton(Hat::RIGHT, 30, 4);
  pushButton(Button::A, 300);
  // 設定 > 本体 > 日付と時刻
  pushHatButtonContinuous(Hat::DOWN, 2000);
  pushHatButton(Hat::RIGHT, 100);
  pushHatButton(Hat::DOWN, 30, 5);
  pushButton(Button::A, 100);
  // 日付と時刻 > 現在の日付と時刻
  pushHatButton(Hat::DOWN, 30, 3);
}

// IDくじ
void ID()
{
  //ロトミ起動 > IDくじ
  pushButton(Button::A, 300, 2);
  pushHatButton(Hat::DOWN, 150);
  pushButton(Button::A, 30, 40);
  pushButton(Button::B, 30, 125);
  // ホーム画面 > 現在の日付と時刻
  goToTimeSetting();
}


void day1day30()
{
  pushButton(Button::A, 100);
  pushHatButton(Hat::RIGHT, 30, 2);
  pushHatButton(Hat::UP, 30);
  pushHatButton(Hat::RIGHT, 30, 3);
  pushButton(Button::A, 30);
}

void day1day30Multi()
{
  pushButton(Button::A, 100);
  pushHatButton(Hat::LEFT, 30, 3);
  pushHatButton(Hat::UP, 30);
  pushHatButton(Hat::RIGHT, 30, 3);
  pushButton(Button::A, 30);
}

void day31day1()
{
  day1day30();
  delay(200);
  pushButton(Button::A, 100);
  pushHatButton(Hat::LEFT, 30, 3);
  pushHatButton(Hat::UP, 30);
  pushHatButton(Hat::RIGHT, 30, 3);
  pushButton(Button::A, 30);
}

int changeDate()
{
  if (day_count == 30) {
    day31day1(); // 30 -> 31 -> 1
    day_count = 1;
    return 1;
  }
  else {
    day1day30();
    return ++day_count;
  }
}

void ID2()
{ // ホーム画面 > ゲーム画面
  pushButton(Button::HOME, 1000, 2);
}

// ワットを回収するシーケンス
void execWattGainSequence()
{
    // 募集開始
    pushButton(Button::A, 3000);
    // 日付変更
    goToTimeSetting();
    day1day30();
    ID2();
    // レイド募集中止
    pushButton(Button::B, 800);
    pushButton(Button::A, 3400);
    // ワット回収
    pushButton(Button::A, 600);
    pushButton(Button::B, 600);
    pushButton(Button::B, 600);
}

// IDくじ > マニア
void moveFromIDToMania()
{

}

// マニア > 掘り出し物市
void moveFromManiaToHoridashi()
{

}

// 掘り出し物市 > マニア
void moveFromHoridashiToMania()
{

}

// マニア > IDくじ
void moveFromManiaToID()
{

}

// 再起動
void restartApp()
{

}


// get button input
int getButton()
{
  if (digitalRead(BTN_IN1_PIN) == LOW) return 0;
  if (digitalRead(BTN_IN2_PIN) == LOW) return 1;
  return -1;
}

// get num input
int getInput(int inputBits)
{
  int in = 0, b = 0, k = 1;
  pinMode(BTN_VCC_PIN, OUTPUT);
  digitalWrite(BTN_VCC_PIN, LOW);
  pinMode(BTN_IN1_PIN, INPUT_PULLUP);
  pinMode(BTN_IN2_PIN, INPUT_PULLUP);
  for (int i = 0; i < inputBits; i++)
  {
    int j = -1;
    while ((j = getButton()) == -1){
      delay(4);
      if (b >= 255) k = -1;
      if (b <= 0) k = 1;
      b += k;
      whiteLED(b);
    }
    if (j == 1) blueLED(1); else greenLED(1);
    tone(SPK_IN_PIN, 880, j == 1 ? 200 : 50);
    while (getButton() != -1){
      delay(4);
      if (b >= 255) k = -1;
      if (b <= 0) k = 1;
      b += k;
      whiteLED(b);
    }
    if (j == 1) blueLED(0); else greenLED(0);
    delay(40);
    in = (in << 1) + j;
  }
  pinMode(BTN_IN1_PIN, OUTPUT);
  pinMode(BTN_IN2_PIN, OUTPUT);
  return in;
}

// show num
void showNum(int num, int outputBits)
{
  delay(100);
  whiteLED(127);
  delay(100);
  for (int i = outputBits; i > 0; i--)
  {
    int j = (num >> (i - 1)) % 2;
    if (j == 1) blueLED(1); else greenLED(1);
    tone(SPK_IN_PIN, 880, j == 1 ? 200 : 50);
    delay(200);
    if (j == 1) blueLED(0); else greenLED(0);
    delay(100);
  }
  delay(100);
  whiteLED(0);
  delay(100);
}

// mode select
void setMode()
{
  mode = getInput(MODE_BITS);
  showNum(mode, MODE_BITS);
}

// reset
void(* resetFunc) (void) = 0; 

void setup()
{
  initAutoCommandUtil();
  pinMode(SPK_GND_PIN, OUTPUT);
  digitalWrite(SPK_GND_PIN, LOW);
  tone(SPK_IN_PIN, 880, 200);
  setMode();
  pushButton(Button::B, 200, 3);
  delay(400);

  // 初めの卵が出現するまで走り回る
  switch (mode)
  {
    case 0: // A連打
      break;
    case 1: // 自動孵化(全自動)
      moveToInitialPlayerPosition();
      runAround(TIME_TO_HATCHING_SEC / 3);
      break;
    case 2: // 自動孵化(ループのみ)
      moveToInitialPlayerPosition();
      break;
    case 3: // 自動ワット稼ぎ
      pushButton(Button::A, 1000);
      break;
    case 4: // 自動レイド
      break;
    case 5: // 自動IDくじ
      break;
    case 6: // 自動IDくじ・掘り出し物市・マニア
      break;
    case 7: // 自動日付変更(無限)
      goToTimeSetting();
      day1day30();
      break;
    case 8:
      break;
    case 9: // 自動ワット稼ぎ(回数指定)
      v0 = getInput(8);
      showNum(v0, 8);
      pushButton(Button::A, 1000);
      for (v1 = 0; v1 < v0; v1++) {
        execWattGainSequence();
      }
      break;
    case 10: // 自動日付変更(10日単位回数指定)
      v0 = getInput(8) * 10;
      showNum(v0, 12);
      goToTimeSetting();
      day1day30();
      for (v1 = 1; v1 < v0; v1++) {
        delay(150);
        day1day30Multi();
      }
      ID2();
      break;
    case 11:
      while (true) {
        pushButton(Button::A, 1000);
        execWattGainSequence();
        execWattGainSequence();
        execWattGainSequence();
        tone(SPK_IN_PIN, 1760, 150);
        delay(200);
        tone(SPK_IN_PIN, 1760, 150);
        delay(200);
        tone(SPK_IN_PIN, 1760, 150);
        v1 = getInput(1);
        if (v1 == 0) {
          pushButton(Button::HOME, 700);
          pushButton(Button::X, 700);
          pushButton(Button::A, 100, 30);
          delay(12500);
          pushButton(Button::A, 100, 10);
          delay(7000);
        }else{
          pushButton(Button::B, 100, 20);
          pushButton(Button::X, 700);
          pushHatButtonContinuous(Hat::LEFT_UP, 1000);
          pushHatButton(Hat::RIGHT, 500);
          pushHatButton(Hat::DOWN, 500);
          pushButton(Button::A, 2000);
          delay(10000);
          pushButton(Button::B, 2000);
          pushButton(Button::A, 2000);
          break;
        }
      }
      break;
    case 12:
      break;
    case 13:
      break;
    case 14:
      break;
    case 15:
      break;
    default:
      resetFunc();  //call reset
      break;
  }
}

void loop()
{
  switch (mode)
  {
    case 0:
      pushButton(Button::A, 30, 10000);
      break;
    case 1:
      execHatchingSequence();
      break;
    case 2:
      runAround(30);
      break;
    case 3:
      execWattGainSequence();
      break;
    case 4:
      execRaidBattleSequence();
      break;
    case 5:
      ID();
      changeDate();
      ID2();
      break;
    case 6:
      ID();
      changeDate();
      ID2();
      break;
    case 7:
    case 8:
      delay(120);
      day1day30Multi();
      break;
    case 15:
      pushButton(Button::A, 500);
      pushButton(Button::B, 500);
      pushButton(Button::X, 500);
      pushButton(Button::Y, 500);
      pushButton(Button::L, 500);
      pushButton(Button::R, 500);
      pushButton(Button::ZL, 500);
      pushButton(Button::ZR, 500);
      pushButton(Button::PLUS, 500);
      pushButton(Button::MINUS, 500);
      pushButton(Button::LCLICK, 500);
      pushButton(Button::RCLICK, 500);
      pushHatButton(Hat::UP, 500);
      pushHatButton(Hat::UP_RIGHT, 500);
      pushHatButton(Hat::RIGHT, 500);
      pushHatButton(Hat::RIGHT_DOWN, 500);
      pushHatButton(Hat::DOWN, 500);
      pushHatButton(Hat::DOWN_LEFT, 500);
      pushHatButton(Hat::LEFT, 500);
      pushHatButton(Hat::LEFT_UP, 500);
      pushHatButton(Hat::CENTER, 500);
      pushHatButtonContinuous(Hat::UP, 2000);
      pushHatButtonContinuous(Hat::UP_RIGHT, 2000);
      pushHatButtonContinuous(Hat::RIGHT, 2000);
      pushHatButtonContinuous(Hat::RIGHT_DOWN, 2000);
      pushHatButtonContinuous(Hat::DOWN, 2000);
      pushHatButtonContinuous(Hat::DOWN_LEFT, 2000);
      pushHatButtonContinuous(Hat::LEFT, 2000);
      pushHatButtonContinuous(Hat::LEFT_UP, 2000);
      pushHatButtonContinuous(Hat::CENTER, 2000);
      tiltJoystick(100, 0, 0, 0, 2000);
      tiltJoystick(0, 100, 0, 0, 2000);
      tiltJoystick(0, 0, 100, 0, 2000);
      tiltJoystick(0, 0, 0, 100, 2000);
      pushButton(Button::HOME, 500);
      pushButton(Button::CAPTURE, 500);
      resetFunc();  //call reset
    default:
      resetFunc();  //call reset
      break;
  }
}
