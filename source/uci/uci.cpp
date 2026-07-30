#include "uci/uci.hpp"
#include "chess/board/position.hpp"
#include "search/search.hpp"
#include "search/history.hpp"
#include "search/killer.hpp"
#include "eval/score.hpp"

#include <thread>
#include <sstream>

namespace Tempo::UCI {

    void infoDepth(int depth, int seldepth, int score, u64 elapsed, u64 totalNodes, const u16 pvArr[], int pvLen) {
        std::cout << "info depth " << depth <<
                        " seldepth " << seldepth <<
                        " score " << scoreString(score) << 
                        " nodes " << totalNodes << 
                        " nps " << totalNodes * 1000 / std::max<u64>(1ULL, elapsed) << 
                        " hashfull " << TranspositionTable::hashfull() <<
                        " time " << std::max<u64>(1ULL, elapsed) <<
                        " pv ";
        
        for (int i = 0; i < pvLen; ++i) 
            std::cout << Move::toString(pvArr[i]) << ' ';
        
        std::cout << std::endl;
    }

    void infoDepth(int depth, u64 elapsed, u64 totalNodes, const u16 currmove, int currmovenumber) {
        std::cout << "info depth " << depth << " time " << elapsed << " nodes " << totalNodes << " nps " << totalNodes * 1000 / std::max<u64>(1ULL, elapsed) << " currmove " << Move::toString(currmove) << " currmovenumber " << currmovenumber << std::endl;
    }
    void infoString(const String& msg) { std::cout << "info string " << msg << std::endl; }

    namespace {

    Position position;
    std::thread searchThread;

    void stop() {
        Search::stop();

        if (searchThread.joinable()) {
            searchThread.join();
        }
    }

    void handleUci() {
        std::cout << "id name Tempo \n" <<
                     "id author GoobusTheNoobus\n" <<
                     "uciok" <<
                     std::endl;    
    }

    void handleGo(std::istringstream& iss) {
        stop();

        if (searchThread.joinable()) {
            searchThread.join();
        }

        String token;

        int depth = 0;
        int movetime = 0;

        int winc = 0;
        int binc = 0;
        int wtime = 0;
        int btime = 0;
        
        while (iss >> token) {
            if (token == "depth") {
                iss >> depth;
            } else if (token == "movetime") {
                iss >> movetime;
            } else if (token == "wtime") {
                iss >> wtime;
            } else if (token == "btime") {
                iss >> btime;
            } else if (token == "winc") {
                iss >> winc;
            } else if (token == "binc") {
                iss >> binc;
            } else if (token == "perft") {
                std::cout << "Perft is not supported anymore because I accidentally deleted it.\n";
            }
        }

        int timeLimit = 0;

        if (movetime > 0) {
            timeLimit = movetime;
        }

        else if (wtime > 0 || btime > 0) {
            int ourTime = position.getSideToMove() == White ? wtime : btime;
            int ourInc  = position.getSideToMove() == White ? winc : binc;

            timeLimit = std::min(ourTime / 20 + ourInc / 2, ourTime);
        }

        if (depth < 1 || depth > Search::MaxSearchDepth) depth = Search::MaxSearchDepth;
        
        searchThread = std::thread([depth, timeLimit]() {
            Search::start(position, depth, timeLimit);
        });
    }

    void handlePosition(std::istringstream& iss) {
        stop();

        String token;
        iss >> token;

        if (token == "startpos") {
            position.parseFen(StartingPositionFen);

            iss >> token;
        } else if (token == "fen") {
            String fen;

            while ((iss >> token) && token != "moves") {
                fen += token + " ";
            }

            position.parseFen(fen);
        }

        if (token == "moves") {
            while (iss >> token) {
                position.makeMove(token);
            }
        }
    }
    void handleIsready() {
        std::cout << "readyok" << std::endl;
    }
    void ucinewgame() {
        TranspositionTable::clear();
        Search::History::clear();
        Search::Killer::clear();
    }

    void dispatch(const String& cmd, std::istringstream& iss) {
        if (cmd == "uci") {
            handleUci();
        }
        else if (cmd == "isready") {
            handleIsready();
        }
        else if (cmd == "go") {
            handleGo(iss);
        }
        else if (cmd == "position") {
            handlePosition(iss);
        }
        else if (cmd == "ucinewgame") {
            ucinewgame();
        }
        else if (cmd == "d") {
            std::cout << position << std::endl;
        }
        else {
            infoString("invalid command");
        }
    }
    }

    void loop() {

        position.setUpStartpos();

        while (true) {
            String command;
            if (!std::getline(std::cin, command)) {
                stop();
                break;
            }
            std::istringstream iss(command);
            String token;

            if (command.find_first_not_of(" ") == String::npos) continue;

            iss >> token;
            if (token == "quit") {
                stop();
                break;
            } else if (token == "stop") {
                stop();
                continue;
            }

            dispatch(token, iss);
        }

        stop();
    }
}