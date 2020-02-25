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

// LEDに使うピン
const uint16_t LED_PIN = 13;

// Buttonに使うピン
const uint16_t BTN_VCC_PIN = 10;
const uint16_t BTN_IN1_PIN = 7;
const uint16_t BTN_IN2_PIN = 11;

// mode
const uint16_t MODE_BITS = 3;
int mode = 0;

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
  int in = 0;
  pinMode(BTN_VCC_PIN, OUTPUT);
  digitalWrite(BTN_VCC_PIN, LOW);
  pinMode(BTN_IN1_PIN, INPUT_PULLUP);
  pinMode(BTN_IN2_PIN, INPUT_PULLUP);
  for (int i = 0; i < inputBits; i++)
  {
    int j;
    while ((j = getButton()) == -1){
      delay(10);
    }
    digitalWrite(LED_PIN, HIGH);
    delay(40 + 160 * j);
    digitalWrite(LED_PIN, LOW);
    while (getButton() != -1){
      delay(10);
    }
    in = (in << 1) + j;
  }
  pinMode(BTN_IN1_PIN, OUTPUT);
  pinMode(BTN_IN2_PIN, OUTPUT);
  return in;
}

// show num
void showNum(int num, int outputBits)
{
  for (int i = outputBits; i > 0; i--)
  {
    int j = (num >> (i - 1)) % 2;
    digitalWrite(LED_PIN, HIGH);
    delay(40 + 160 * j);
    digitalWrite(LED_PIN, LOW);
    delay(200 - 160 * j);
  }
  delay(400);
}

// mode select
void setMode()
{
  mode = getInput(MODE_BITS);
  showNum(mode, MODE_BITS);
}

void setup()
{
  initAutoCommandUtil();
  pushButton(Button::B, 300, 3);
  setMode();
  pushButton(Button::B, 300, 3);
  delay(500);

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
    case 3:
      break;
    default:
      break;
  }
}

void loop()
{
  switch (mode)
  {
    case 0:
      pushButton(Button::A, 20, 10000);
      break;
    case 1:
      execHatchingSequence();
      break;
    case 2:
      runAround(30);
      break;
    case 3:
      break;
    default:
      break;
  }
}
