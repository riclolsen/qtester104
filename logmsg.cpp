/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010-present Ricardo L. Olsen
 *
 * Disclaimer
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "logmsg.h"
#include <QMutexLocker>

using namespace std;

TLogMsg::TLogMsg()
{
    mMaxMsg = 1000;
    mDoLog = true;
    mRegTime = false;
    mLevel = 0;
}

void TLogMsg::setMaxMsg(unsigned int maxmsg)
{
    QMutexLocker locker(&mMutex);
    mMaxMsg = maxmsg;
}

void TLogMsg::setLevel(unsigned int level)
{
    QMutexLocker locker(&mMutex);
    mLevel = level;
}

void TLogMsg::activateLog()
{
    QMutexLocker locker(&mMutex);
    mDoLog = true;
}

void TLogMsg::deactivateLog()
{
    QMutexLocker locker(&mMutex);
    mLstLog.clear(); // clean lists
    mLstTime.clear();
    mDoLog = false;
}

void TLogMsg::doLogTime()
{
    QMutexLocker locker(&mMutex);
    mLstLog.clear(); // clean lists, sync
    mLstTime.clear();
    mRegTime = true;
}

void TLogMsg::dontLogTime()
{
    QMutexLocker locker(&mMutex);
    mRegTime = false;
}

bool TLogMsg::haveMsg()
{
    QMutexLocker locker(&mMutex);
    return !mLstLog.empty();
}

bool TLogMsg::isLogging()
{
    QMutexLocker locker(&mMutex);
    return mDoLog;
}

// coloca a mensagem na fila
void TLogMsg::pushMsg( const char * msg, unsigned int level )
{
    QMutexLocker locker(&mMutex);
    if ( mDoLog && ( mLstLog.size() < mMaxMsg ) && ( mLevel <= level ) ) {
        mLstLog.push_back( msg );
        if ( mRegTime ) { // coloca hora na fila, se for o caso
            mLstTime.push_back( time( NULL ) );
        }
    }
}

int TLogMsg::count()
{
    QMutexLocker locker(&mMutex);
    return int(mLstLog.size());
}

// Tira mensagem da fila
string TLogMsg::pullMsg()
{
    QMutexLocker locker(&mMutex);
    if ( mLstLog.empty() || !mDoLog )
        return "";

    string s = mLstLog.front();   // pega a primeira da fila
    mLstLog.pop_front();          // retira-a da fila

    // se tem registro de hora, pega a hora e formata para exibir antes da mensagem
    if (mRegTime){
        char buffer [201];
        static time_t hora_ant;
        time_t hora = mLstTime.front();
        mLstTime.pop_front();
        if (hora != hora_ant)
          {
          struct tm * timeinfo;
          timeinfo = localtime ( &hora );
          // strftime ( buffer,200,"%d/%m %H:%M:%S ",timeinfo );
          strftime ( buffer,200,"%H:%M:%S ",timeinfo );
          s = buffer + s;
          }
        else
          s = "         " + s;
        hora_ant = hora;
    }

    return s;
}

