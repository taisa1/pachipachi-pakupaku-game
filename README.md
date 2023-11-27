# pachpachi-pakupaku-game
![screenshot](screenshot.png)
画面奥の顔に向かってアイテムが降ってくるので、それを集めるゲームです。黄色いアイテムに目の位置を合わせて瞬き（「ぱちぱち」）、緑のアイテムに口の位置を合わせて開閉（「ぱくぱく」）させるとゲットできます。操作はカメラで認識した顔自体で行います。

## 遊び方
```
git clone git@github.com:taisa1/pachipachi-pakupaku-game.git
cmake .
make
./main
```

## 要求ライブラリ
- OpenCV
- OpenGL
- LAPACK
- dlib

Ubuntu20.04 + OpenCV 4.2.0 + OpenGL 4.6 + LAPACK 3.9.0 + dlib 19.24 で正常な動作を確認しています。

