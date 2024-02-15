// Copyright (c) 2016 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/******************************************************************************
 * Copyright © 2014-2019 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

#include "uint256.h"

#include <atomic>
#include <mutex>
#include <string>

struct AtomicCounter {
    std::atomic<uint64_t> value;

    AtomicCounter() : value {0} { }

    void increment() {
        ++value;
    }

    void decrement() {
        --value;
    }

    uint64_t get() const {
        return value.load();
    }
};

class AtomicTimer {
  private:
    std::mutex mtx;
    uint64_t threads;
    int64_t start_time;
    int64_t total_time;

  public:
    AtomicTimer() : threads(0), start_time(0), total_time(0) {}

    /**
     * Starts timing on first call, and counts the number of calls.
     */
    void start();

    /**
     * Counts number of calls, and stops timing after it has been called as
     * many times as start().
     */
    void stop();

    bool running();

    uint64_t threadCount();

    double rate(const AtomicCounter& count);
    double rate(const int64_t count);

};

extern AtomicCounter transactionsValidated;
extern AtomicCounter ehSolverRuns;
extern AtomicCounter solutionTargetChecks;
extern AtomicTimer miningTimer;

void TrackMinedBlock(uint256 hash);

void MarkStartTime();
double GetLocalSolPS();
int EstimateNetHeightInner(int height, int64_t tipmediantime,
                           int heightLastCheckpoint, int64_t timeLastCheckpoint,
                           int64_t genesisTime, int64_t targetSpacing);

void TriggerRefresh();

void ConnectMetricsScreen();
void ThreadShowMetricsScreen();

/**
 * Rendering options:
 * KomodoLogo: img2txt -W 40 -H 20 -f utf8 -d none qt/res/icons/komodo.png
 */
const std::string METRICS_ART =
    "              [0;1;36;96;47m8[0;37;5;46;106m@[0;36;5;46;106m%ttttt;;[0;37;5;46;106m@@[0m              \n"
    "          [0;36;5;46;106m8[0;1;36;96;46m8[0;36;5;46;106m88@@XSS%ttttttttt[0;37;5;46;106mS[0;1;36;96;47mX[0m         \n"
    "       [0;37;46mX[0;1;36;96;46mX@8888[0;36;5;46;106m8[0;37;5;46;106m8[0;1;36;96;47m8S[0;1;37;97;47m8888[0;1;36;96;47mS[0;37;5;46;106mX[0;36;5;46;106m;ttttttt;[0m       \n"
    "     [0;37;46mS[0;1;30;90;46m;;[0;1;36;96;46mSXX[0;1;36;96;47mX[0;1;37;97;47m8[0;37;5;47;107m;          t[0;36;5;46;106mttttttttt;[0m     \n"
    "   [0;36;47m@[0;1;30;90;46m;;;;%[0;1;36;96;47mS[0;37;5;47;107m;  .S[0;1;37;97;47m8[0;1;36;96;47m8[0;36;5;46;106m888@XS[0;37;5;46;106mX[0;36;5;46;106mS%%[0;37;5;46;106mX[0;37;5;47;107m:.;[0;37;5;46;106m@[0;36;5;46;106mtttt[0;37;5;46;106m@[0m   \n"
    "  [0;1;30;90;46m8ttt;[0;36;47m8[0;37;5;47;107m;  t[0;1;36;96;47mX[0;1;36;96;46mXX@@8888[0;36;5;46;106m88@XX%[0;37;5;47;107m    ;[0;36;5;46;106mttttt;[0m  \n"
    " [0;36;47m8[0;1;30;90;46m%%tt[0;36;47mS[0;37;5;47;107m. .[0;1;36;96;47mS[0;1;30;90;46mt;;[0;1;36;96;46mSSX@@8888[0;36;5;46;106m88X[0;37;5;47;107m;;[0;1;37;97;47m88[0;37;5;46;106m@[0;36;5;46;106mttttttt[0;37;5;46;106mX[0m \n"
    " [0;1;30;90;46mS%%%[0;36;47m8[0;37;5;47;107m  .[0;36;47m8[0;1;30;90;46mt;;;;;[0;37;46mX[0;1;37;97;47m88[0;37;5;47;107m%tS[0;1;37;97;47m8[0;37;5;47;107mS;;[0;37;5;46;106mX[0;36;5;46;106m@@XSS[0;37;5;46;106m8@[0;36;5;46;106m;tttt[0m \n"
    "[0;36;5;40;100m;[0;1;30;90;46mSSS%[0;37;5;47;107mS  [0;1;37;97;47m%[0;1;30;90;46mtttt;%[0;1;37;97;47m8[0;37;5;47;107m         [0;1;36;96;47m8[0;1;36;96;46m8[0;36;5;46;106m888@[0;37;5;46;106m8[0;37;5;47;107m  :[0;36;5;46;106mtttt[0;37;5;46;106mX[0m\n"
    "[0;1;30;90;46m@SSS8[0;37;5;47;107m   [0;1;30;90;46m8%%ttt[0;1;36;96;47m%[0;37;5;47;107m          [0;1;37;97;47m8[0;1;36;96;46m888[0;36;5;46;106m88@[0;37;5;47;107m   [0;36;5;46;106m%%%tt[0m\n"
    "[0;36;5;40;100m88[0;1;30;90;46mXS8[0;37;5;47;107m   [0;1;30;90;46m8%%%%t[0;1;30;90;47m [0;37;5;47;107m          [0;1;37;97;47m8[0;1;36;96;46m@@888[0;36;5;46;106m8[0;37;5;47;107m   [0;36;5;46;106mSXS%t[0m\n"
    "[0;36;5;40;100m;888[0;1;30;90;46mX[0;37;5;47;107mX  [0;1;30;90;47m [0;1;30;90;46mS%%%%S[0;1;37;97;47m8[0;37;5;47;107m        [0;1;37;97;47m8[0;1;36;96;46mSSX@@8[0;1;36;96;47mX[0;37;5;47;107m  :[0;36;5;46;106m8@@X[0;37;5;46;106mX[0m\n"
    " [0;36;5;40;100m8888[0;1;30;90;47m8[0;37;5;47;107m  .[0;36;5;40;100m.[0;1;30;90;46mSSS%%%8[0;1;37;97;47mt8[0;37;5;47;107mSS[0;1;37;97;47m8X[0;37;46m@[0;1;30;90;46m;;;[0;1;36;96;46mSSX[0;36;47m8[0;37;5;47;107m.  [0;1;36;96;47m8[0;1;36;96;46m8[0;36;5;46;106m88@[0m \n"
    " [0;36;5;40;100m 8888[0;1;30;90;47mt[0;37;5;47;107m  :[0;1;30;90;47mt[0;1;30;90;46mXSSS%%%%%tttt;;;;;[0;1;36;96;47m@[0;37;5;47;107m.  [0;1;37;97;47m8[0;1;36;96;46m@888[0;37;5;46;106m@[0m \n"
    "  [0;36;5;40;100mt8888[0;37;5;40;100m8[0;37;5;47;107mt  @[0;1;30;90;47mS[0;1;30;90;46m8SSS%%%%%ttttS[0;36;47m@[0;37;5;47;107m%  :[0;1;36;96;47mX[0;1;36;96;46mXX@@[0;36;5;46;106m8[0m  \n"
    "   [0;1;30;90;47m8[0;36;5;40;100m88888[0;1;30;90;47m;[0;37;5;47;107mt  :[0;1;37;97;47m@[0;1;30;90;47m.@[0;1;30;90;46m88XX88[0;36;47m8[0;1;30;90;47m [0;1;37;97;47m8[0;37;5;47;107m:  ;[0;1;37;97;47mX[0;1;30;90;46mX;;[0;1;36;96;46mSX[0;1;36;96;47mX[0m   \n"
    "     [0;36;5;40;100m@88888[0;37;5;40;100m8[0;1;37;97;47m;[0;37;5;47;107mS            t[0;1;37;97;47m8[0;36;47m8[0;1;30;90;46m%;;;;8[0m     \n"
    "       [0;36;5;40;100mS8888888 [0;1;30;90;47mS:[0;1;37;97;47m:tt;[0;1;30;90;47m :[0;36;47m8[0;1;30;90;46m8%%tttt8[0m       \n"
    "         [0;1;30;90;47mt[0;36;5;40;100m%88888888[0;1;30;90;46mXXSSSSS%%%8[0;1;30;90;47m [0m         \n"
    "              [0;1;30;90;47m@[0;36;5;40;100m.S888888X.[0;1;30;90;47m%[0m              \n";
