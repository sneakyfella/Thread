/* This source file is part of the concurrent Fiber based Framework.
*
* Copyright (c) 2015-2016 Joshua Chew
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include "ForwardDeclarations.h"
#include "DelegateImpl.h"

#include <vector>

#define E_METHOD(classType, classMethod)\
	classType,&classType::classMethod

template<class RType>
class Event;

template<typename RType, typename... Args>
class Event< RType(Args...) >
{
    /***** Types *****/
public:
    //!< Event delegate type.
    typedef typename Delegate< RType(Args...) > DelegateType;

    /***** Public Methods *****/
public:
    inline Event() {}
    inline ~Event() {}
    //! Raise an event with arguments.
    void Raise(Args... args)
    {
        if (mDelegates.size() > 0) {
            for (size_t i = 0; i < mDelegates.size(); ++i) {
                typename DelegateType dlg = mDelegates[i];
                if (dlg)
                    dlg(args...);
            }

            if (mSweep)
                Sweep();
        }
    }
    //! Raise an event with arguments using the () operator.
    inline void operator () (Args... args)
    {
        Raise(args...);
    }
    //! Register a listener function to the event.
    inline void Register(typename DelegateType dlg)
    {
        mDelegates.push_back(dlg);
    }

    //! Increment operator used to register a listener function to the event.
    inline void operator += (const typename DelegateType & dlg)
    {
        Register(dlg);
    }
    //! Unregister a listener function from the event.
    void Unregister(const typename DelegateType & dlg)
    {
        for (unsigned int i = 0; i < mDelegates.size(); ++i) {
            if (mDelegates[i] == dlg) {
                mDelegates[i].Reset();
                mSweep = true;
                break;
            }
        }
    }
    //! Decrement operator used to unregister a listener function from the event.
    inline void operator -= (const typename DelegateType & dlg)
    {
        Unregister(dlg);
    }
    //! Clears all registered event listeners.
    inline void Clear()
    {
        mDelegates.clear();
        mSweep = false;
    }

    /***** Static Public Methods *****/
public:
    //! Wrapper function for getting a function delegate.
    template< RType(*OMethod)(Args...) >
    inline static typename DelegateType Function()
    {
        return DelegateType::FromFunction<OMethod>();
    }
    //! Wrapper function for getting a class method delegate.
    template< class O, RType(O::*OMethod)(Args...) >
    inline static typename DelegateType Method(O * objectPtr)
    {
        return DelegateType::FromMethod< O, OMethod >(objectPtr);
    }
    //! Wrapper function for getting a class const method delegate.
    template< class O, RType(O::*OMethod)(Args...) const >
    inline static typename DelegateType CMethod(const O * objectPtr)
    {
        return DelegateType::FromConstMethod< O, OMethod >(objectPtr);
    }

    /***** Private Methods *****/
private:
    void Sweep()
    {
        for (size_t i = 0; i < mDelegates.size(); ++i) {
            if (!mDelegates[i]) {
                mDelegates[i] = mDelegates[mDelegates.size() - 1];
                mDelegates.pop_back();
            }
        }
    }

    /***** Private Members *****/
private:
    std::vector<typename DelegateType> mDelegates;
    bool                               mSweep;
};

