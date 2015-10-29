銀の弾などない
==============

## 東京高専競技部門
第26回 全国高等専門学校プログラミングコンテスト(長野大会)の競技部門で実際に使用されたソースコード．

## 順位
### 26th-procon
どの学校がNAPROCKか知らないから，誰か修正しておいて

### NAPROCK2014
13位

## 開発環境
LinuxのQtCreator依存．MSVCが頭良ければ，Windowsでも行けるのでは？

* (Arch|Gentoo|Kubuntu) Linux
    * gcc 5.2.0

外部ライブラリとしては，以下の2つを利用しています．
導入方法については，[昨年のREADME.md](://github.com/tnct-spc/procon2014/blob/master/README.md)を見てください．
* Boost C++ Libraries
* Qt
* qhttpserver
* BoostConnect (./lib/boostconnectにgit-submodule済)

## 実行方法
1. QtCreatorを起動後，全プロジェクトをビルド．
1. masterやらslaveやらoutside_serverやらを立ち上げれば良い．先にサーバを立ち上げ，問題を起動しておく必要がある．

##ディレクトリ構造

| パス              | 説明 |
|:------------------|:-----|
| ./master/         | 解答をHTTPで集計し，良い解答を提出する |
| ./slave/          | 解答を提出するクライアント本体 |
| ./tengu/          | 実際に問題をとくアルゴリズム |
| ./takao/          | tenguで使うアルゴリズムライブラリ |
| ./muen_zuka/      | ネットワークやその他メインのプログラムで使用するもの |
| ./outside_server/ | 外部大会サーバー互換機 |
| ./auto_evaluter/  | 問題を自動で回して，アルゴリズムのデータを採る |
| ./test/           | 各種テスト |
| ./lib/            | ライブラリ |

## ライセンスについて
昨年同様，MITにしようかと思ったのですが，
Qtライブラリのコードを継承している部分があるので，
念の為，LGPLで公開しようと思います．

