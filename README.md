# ホビーRPN電卓

TOKYO FLIP-FLOP「ホビーRPN電卓」の製品版ファームウェアです。

公式サイト：https://www.hobby-rpn.com/

## ファームウェア書き換えについて

ホビーRPN電卓は、ユーザ側でファームウェアを書き換えられる仕組みを持っています。オリジナルのファームウェアを改変して書き込むことで、自分の欲しい機能を新たに追加するといったことが可能です。

ただし、**書き換えは自己責任**でお願いします。また本機能は、ある程度ArduinoやAVRマイコンが触れる方の使用を想定しています。万が一動かなくなった場合の保証はできませんのでご了承ください。

## 基本情報

- マイコン：ATmega328P-AU
- 動作クロック：8MHz（内蔵クロック）

基板裏面の右上に6つのパッドがあり、これがファーム書き換え用の端子になっています。いわゆるICSP方式でのプログラミングが可能です。

<img src="https://user-images.githubusercontent.com/23148662/126727764-eae8069f-04e9-472b-8446-dd21ddb3c412.png" width="640">

それぞれの端子が、マイコン（ATmega328P-AU）の下記端子に接続されています。これを使って、通常のAVRマイコンを書き換える手順でファームアップが可能です。

| 基板側の端子 | マイコン側の端子(ピン番号) |
| --- | --- |
| GND | GND |
| VCC | VCC |
| RST | PC6 / RESET (29) |
| SCK | PB5 / SCK (17) |
| MISO | PB4 / MISO (16) |
| MOSI | PB3 / MOSI (15) |


## 書き換え方

マイコンはArduinoブートローダ未搭載ですので、書き込みには (a) **AVRライタ化したArduino**、もしくは (b) **AVRライタ**が必要です。

### (a) AVRライタ化したArduinoを使う方法

Arduinoユーザの場合、こっちの方がお手軽です。

ホビーRPN電卓ユーザの方が手順を書いてくれていますので、[そちらをご参照ください](https://github.com/sh1ura/hobby-rpn/blob/main/%E6%9B%B8%E3%81%8D%E8%BE%BC%E3%81%BF%E6%96%B9.md)。また検索するとやり方を書いたページがたくさん出てきます。

### (b) AVRライタを使う方法

#### 1. AVRライタの入手

いろんなAVRライタがありますが、ここでは「AVRISP mkII（互換機）」を使います。Amazonなどで入手可能です。

<img src="https://user-images.githubusercontent.com/23148662/126729843-bcec1e9a-1741-457e-97cb-e9959f93b2ce.png" width="640">

またハンダ付けなしで書き込めるようにするため、「ICテストクリップ」の端子を露出させたものを使うと便利です。

<img src="https://user-images.githubusercontent.com/23148662/126730203-358dbcb9-8386-4659-8dd2-6dff6ab1a4d8.png" width="640">

これでパッドを挟み込むことで、お手軽にライタの接続が可能になります。[秋月電子通商](https://akizukidenshi.com/catalog/g/gC-04753/)などで入手可能です。

もちろんパッドにピンヘッダをハンダ付けする方法でも大丈夫です。接触不良が起きにくいため、確実にやるならハンダ付けの方がいいです。

#### 2. AVRライタのセットアップ

Windows10ではドライバが上手くインストールできない場合が多いです。
AVRISP mkII（互換機）の場合は、[こちらの方法](https://qiita.com/yoshihiroo/items/70357cfc3a266f1970be)で上手くいきました。

#### 3. Arduino IDEの設定

- Arduino IDE の環境設定を開き、「追加のボードマネージャのURL」に以下のURLを設定
  - https://kimio-kosaka.github.io/ATMegaCore/package_ATMegaCore_index.json

<img src="https://user-images.githubusercontent.com/23148662/126758417-861760fb-9594-41ad-a13c-664eb2cfa198.png" width="640">

- Arduino IDE のボードマネージャを開く

<img src="https://user-images.githubusercontent.com/23148662/126731579-48cafdc1-0621-45b2-a781-eab199db0c74.png" width="640">

- ATMegaCore (by Kosaka.Lab.) をインストール

<img src="https://user-images.githubusercontent.com/23148662/126731672-0fb9bdba-daea-4385-a8d5-8c0d07d1e4ab.png" width="640">

- 「ボード」で、「ATmega Universal」→「ATmega328P」を選択

<img src="https://user-images.githubusercontent.com/23148662/126731704-3bad4330-011d-4e87-9d59-152ae543e1b9.png" width="640">

- 「Clock」で、「8MHz (Internal)」を選択

<img src="https://user-images.githubusercontent.com/23148662/126731830-b6e62240-4e7d-4822-ab14-80ab2b76d7f9.png" width="640">

- 「書込装置」で、「AVRISP mkII」を選択

<img src="https://user-images.githubusercontent.com/23148662/126731970-3eef76f9-05ab-4378-8080-1f0ed11908ed.png" width="640">

#### 4. 基板とAVRライタの接続

前述のピン配置を参考に、基板とAVRライタを接続してください。ICテストクリップを使う場合、そのままでは挟む力が弱いため、間に何か挟むか手で押さえるなど工夫が必要です（書き込みエラーが出る場合は大半が接触不良です）。

このとき電源はAVRライタから供給されますので、**ホビーRPN電卓のボタン電池は抜いてから**作業して下さい。

<img src="https://user-images.githubusercontent.com/23148662/126732583-22af8e18-6ee4-4523-8ab6-d427b21ff7eb.png" width="640">

#### 5. ファームウェアの書き込み

「スケッチ」→「書込装置を使って書き込む」を選択すると、書き込みが開始されます。

<img src="https://user-images.githubusercontent.com/23148662/126732737-ed09737a-6005-4ac1-aac7-c7ba7793fb71.png" width="640">

「ボードへの書き込みが完了しました。」と出れば成功です。

## キーマトリクス・OLEDの配線について

キーマトリクスについては、オリジナルのソースコードを流用することで、ハード構成を気にせずに使うことができます。key_scan() という関数がある箇所です。

OLEDディスプレイについては、SCLがマイコンの28ピン(PC5/SCL)、SDAが27ピン(PC4/SDA)に接続されています。ハードウェアシリアルを使って制御可能です。

## ユーザー作成ファームウェア

ホビーRPN電卓ユーザの方が作成されたファームウェアです。新しく作られた方がいましたら是非[Twitter](https://twitter.com/tokyo_ff)でお知らせ下さい。

- [ホビーRPN電卓 (Ypsilon project版)](https://github.com/ypsilon-takai/hobby-rpn)
- [ホビーRPN電卓 (u8g版, 誤差なし分数計算版)](https://github.com/sh1ura/hobby-rpn)
