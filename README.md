# Alea jacta est

"Alea jacta est."はオープンソースの連続音声認識エンジン[Julius](http://julius.sourceforge.jp/)を利用した、音声認識による将棋プログラムです。棋譜での表記のように「7六歩」などと喋るとそれを認識して駒を動かします。

## 名前について

"Alea jacta est."は古典ラテン語で「賽は投げられた」という意味でガイウス・ユリウス・カエサルが軍を率いてルビコン川を渡る際に言ったとされています。Juliusにちなんでいます。

# ビルド

https://github.com/julius-speech/julius/releases から julius-4.3.1-linuxbin.tar.gz をダウンロードして

    tar xzvf julius-4.3.1-linuxbin.tar.gz
	cd julius-4.3.1-linuxbin
	git clone https://github.com/umedaikiti/alea-jacta-est.git
	cd alea-jacta-est
	make

としてビルドしてください。

その後 https://github.com/julius-speech/dictation-kit の model を alea-jacta-est のディレクトリに置いてください。

# 遊び方

マイクに向かって駒の動かし方を話してください。駒の動かし方は[棋譜の表記方法](http://www.shogi.or.jp/faq/kihuhyouki.html)にしたがって指定してください。

持ち駒ももちろん使えます。

詰みの判定や対局終了の判断はしていないので玉が取られたとしても続きます。終了するときはCtrl-Cで強制終了してください。

## 動かし方の指示の簡単な説明

### 駒の相対位置
- 右＝指す側から見て右側の駒を動かした場合
- 左＝指す側から見て左側の駒を動かした場合

### 駒の「動作」
- 上＝1段以上、上に動く
- 寄＝1マス以上、横に動く
- 引＝1段以上、下に動く

### その他
* 相手の1手前の指し手と同地点に移動した場合（＝その駒を取った場合）、「同」と記入。
* 到達地点に複数の同じ駒が動ける場合、動作でどの駒が動いたかわからない場合は、「上」または「寄」または「引」を記入します。
* 到達地点に2枚の同じ駒が動ける場合、動作でどの駒が動いたかわからない時は、「左」「右」を記入します。
* 竜や馬が2枚の場合は動作を優先します。ただし、「直」は使わずに「左」「右」で記入します。
* 持ち駒を打つときでも、そこに動ける駒が他にないときは「打」をつける必要はありません。
* 駒が成れるときは必ず「成」「不成」をつけます。1二の歩を1一に動かす時のように、成らないと反則になる場合でも「成」はつけます。 

# 読み方

読み方の流儀が異なるかもしれないので念の為に読み方を列挙しておきます。
- 1 : いち
- 2 : に
- 3 : さん
- 4 : よん
- 5 : ご
- 6 : ろく
- 7 : なな
- 8 : はち
- 9 : きゅう
- 金 : きん
- 銀 : ぎん
- 全 : なりぎん
- 圭 : なりけい
- 杏 : なりきょう
- と : と
- 桂 : けいま
- 玉 : ぎょく
- 香 : きょうしゃ
- 歩 : ふ
- 飛 : ひしゃ
- 龍 : りゅう
- 角 : かく
- 馬 : うま
- 不成 : ならず
- 成 : なり
- 打 : うつ
- 直 : すぐ
- 寄 : よる
- 引 : ひく
- 上 : あがる
- 左 : ひだり
- 右 : みぎ

# 説明

"Alea jacta est."は音声認識を行うためにJuliusライブラリのAPIを内部で呼び出しています。

「7六歩」「同歩」「8二金寄」のような指示を認識させるために語彙と文法を記述したファイルを作成し、プログラムを起動した時にJuliusに読み込ませています。そしてマイクからの音声をJuliusライブラリによって認識し、その結果を文字列の形で受け取ります。

その次は受け取った結果を解析しどこの駒をどこに移動させるのかを決定する必要があります。「どこに」移動させるかは簡単にわかるのですが「どこの」駒を移動させるかは自明ではありません。歩兵のように動きが単純ならば良いのですが、金のようにいろいろな方向に動ける駒は同じ場所に移動できる駒が2つ以上存在する可能性があります。そのため、「どこの」駒を動かすか決める際には、目的地へ行くことができる駒を全て探した後に指示と一致するものを選択しています。

