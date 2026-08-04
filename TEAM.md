
## 担当

```mermaid
graph LR
    NC["nc / irssi"] <--> A
    A["<b>A：運搬層</b><br/>main.cpp<br/>Server.cpp<br/>Client.*<br/>Makefile"] <--> B["<b>B：意味層</b><br/>Commands.cpp<br/>Channel.*<br/>Irc.hpp<br/>README.md"]

    style A fill:#e8f0f8,stroke:#2f6b8f,color:#000
    style B fill:#f8ece4,stroke:#b8562f,color:#000
```

- A = socket / poll / バッファ　B = パーサ / コマンド / チャンネル
- 各ファイルの 1 行目に `// A's part` / `// B's part`。どっちが書くかはそれを見る
- **共有ファイルは `inc/Server.hpp` だけ。** 触る前に一言、追加のみ、整形しない

## 注意点

| | |
|---|---|
| `execute(client, line)` | A→B。`line` は改行を除いた完全な 1 行 |
| `push(message)` | B→A。CRLF は push が付ける。send はしない |
| `quit(client, reason, ok)` | B→A。close の判断は A |

## 進め方

```mermaid
flowchart LR
    D0(["Day0 ✅"]) --> A4 & B3

    A4["A-1〜4<br/>引数・signal<br/>socket〜listen<br/>poll ループ<br/>受信バッファ"]
    B3["B-1〜3<br/>数値リプライ写経<br/>Channel クラス<br/>split パーサ<br/><i>Server 不要</i>"]

    A4 & B3 --> M1{{"合流① echo サーバ<br/>ダミー execute を差し替え"}}

    M1 --> A5["A-5 送信キュー・POLLOUT"] --> M2
    M1 --> B4["B-4 コマンド表<br/>PASS/NICK/USER/001-004"] --> M2

    M2{{"合流② irssi が繋がる<br/>★最初の山"}}

    M2 --> A6["A-6〜7<br/>遅延クローズ<br/>デストラクタ・valgrind"] --> F
    M2 --> B5["B-5〜9<br/>JOIN/PART → PRIVMSG<br/>→ TOPIC/INVITE/KICK<br/>→ MODE → PING"] --> F

    F(["仕上げ：評価シート総当たり<br/>+ 逆説明セッション"])

    style D0 fill:#d8f0e0,stroke:#2d6a4f,color:#000
    style M1 fill:#fff6d8,stroke:#9a6700,color:#000
    style M2 fill:#fff6d8,stroke:#9a6700,color:#000
    style F fill:#f8dcdc,stroke:#a03030,color:#000
    style A4 fill:#e8f0f8,stroke:#2f6b8f,color:#000
    style A5 fill:#e8f0f8,stroke:#2f6b8f,color:#000
    style A6 fill:#e8f0f8,stroke:#2f6b8f,color:#000
    style B3 fill:#f8ece4,stroke:#b8562f,color:#000
    style B4 fill:#f8ece4,stroke:#b8562f,color:#000
    style B5 fill:#f8ece4,stroke:#b8562f,color:#000
```

**すり合わせは2か所** あとは各自のブランチで進める。1 日 1 回ぐらいでmainに合流。

## 動作確認

```sh
ss -lnt | grep 6667                  # LISTEN。待機中 top で CPU 0%
nc -C localhost 6667                 # PASS→NICK→USER で 001〜004 が4行
{ printf 'NI'; sleep 3; printf 'CK a\r\n'; } | nc -q1 localhost 6667   # 分割送信
printf 'PASS wrong\r\n' | nc -q1 localhost 6667   # 464 が出てから切れる
nc を Ctrl-Z → 2万行流す → fg        # 全部届く。他clientは止まらない
valgrind --leak-check=full ./ircserv 6667 pass    # 接続張ったままCtrl-C
```

## 提出前

```sh
grep -rn '::poll' src/                # 1箇所
grep -rn 'errno\|fork\|select' src/   # 0件
grep -rn 'echo:' src/                 # 0件（ダミー消し忘れ）
make && make                          # 2回目は何もしない
```
