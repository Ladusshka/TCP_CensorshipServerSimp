# TCP_CensorshipServerSimp
Text Censorship Server

Language: C/C++ (using socket libraries)

The task is to implement a text censorship server that operates over a TCP connection. The server receives a list of forbidden words, followed by a text, and responds with the same text where every character of any forbidden word is replaced by the '-' character.

The server does not need to handle multiple clients simultaneously in this phase. However, it should be able to handle multiple clients sequentially—one after another.


Protocol Overview

Communication between the client and the server occurs in two distinct phases:

    Forbidden Words Phase
    Censorship Phase

Each phase is terminated by the ASCII character 0x1F. After the Forbidden Words Phase ends, the Censorship Phase immediately begins. When the Censorship Phase ends, the communication terminates.


Phase 1: Sending Forbidden Words

    After the client connects to the server, it sends the list of forbidden words.
    The words are separated by the ASCII character 0x1E.
    This phase ends with the ASCII character 0x1F.

Phase 2: Censorship

    After the server has received the forbidden words, the client sends the text to be censored.
    The text is sent in blocks of up to 1000 characters each (including the terminating character for the block).
    These blocks are separated by the ASCII character 0x1E, and the entire Censorship Phase ends with 0x1F.
    The server must replace each character of any forbidden word exactly matching (case-sensitive) the forbidden words with the '-' character.
    After processing each block, the server sends back the censored block, also separated by 0x1E, with the final block ending at 0x1F.

Important:

    Forbidden words contain only uppercase and lowercase letters.
    The server censors exact sequences of characters. This means:
    If "pes" is forbidden, then "harapes" becomes "hara---", "pesky" becomes "---ky".
    However, "psa", "pEs", or "haraPES" would not be censored because they do not match exactly (case differs or the sequence is different).

Example Communication

Below is an example of the interaction between the client and the server, demonstrating how partial sends (fragmentation) and merged sends can occur.

Client sends (Forbidden Words):

ipsum0x1Egue0x1Eto

(...some time and partial data...)

r0x1F

(Now the server knows the forbidden words: "ipsum", "gue", "tor")

Client then sends (Text to Censor):

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisq
ue auctor neque congue tortor viverra, vel molestie ipsum mollis.0x1E

Server responds (Censored Block):

Lorem ----- dolor sit amet, consectetur adipiscing elit. Quisque auctor neque con--- ------ viverra, vel molestie ipsum mollis.0x1E

(Forbidden words "ipsum", "gue", and "tor" have been censored in this block.)

Client then sends the next block (Text to Censor):

Nullam nibh purus, molestie at eleifend quis, finibus a leo.0x1F

Server responds (Censored Block):

Nullam nibh purus, molestie at eleifend quis, finibus a leo.0x1F

(No forbidden words found, so no changes were made.)

After sending 0x1F, the client and server end the communication.



Sample Client

TCPclient.out <server IP or hostname> <server port> <optional test seed>

The sample client implements random texts for censorship. Tests are pseudo-random. If you want to repeat an identical test, you can do so by specifying the seed (which the client prints upon startup).

  



