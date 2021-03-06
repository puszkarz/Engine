/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

/*
 Copyright (C) 2008, 2009 Jose Aparicio
 Copyright (C) 2008 Chris Kenyon
 Copyright (C) 2008 Roland Lichters
 Copyright (C) 2008 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file defaultprobabilityhelpers.hpp
    \brief bootstrap helpers for default-probability term structures
     \ingroup termstructures
*/

#ifndef quantext_default_probability_helpers_hpp
#define quantext_default_probability_helpers_hpp

#include <ql/termstructures/bootstraphelper.hpp>
#include <ql/termstructures/defaulttermstructure.hpp>
#include <ql/time/schedule.hpp>
#include <qle/instruments/creditdefaultswap.hpp>

namespace QuantExt {

//! alias for default-probability bootstrap helpers
typedef BootstrapHelper<DefaultProbabilityTermStructure> DefaultProbabilityHelper;
typedef RelativeDateBootstrapHelper<DefaultProbabilityTermStructure> RelativeDateDefaultProbabilityHelper;

/*! Base default-probability bootstrap helper
    @param tenor  CDS tenor.
    @param frequency  Coupon frequency.
    @param settlementDays  The number of days from today's date
                           to the start of the protection period.
                           Does not refer to initial cash settlements
                           (upfront and/or rebates) which are typically
                           on T+3
    @param paymentConvention The payment convention applied to
                             coupons schedules, settlement dates
                             and protection period calculations.
     \ingroup termstructures
*/
class CdsHelper : public RelativeDateDefaultProbabilityHelper {
public:
    CdsHelper(const Handle<Quote>& quote, const Period& tenor, Integer settlementDays, const Calendar& calendar,
              Frequency frequency, BusinessDayConvention paymentConvention, DateGeneration::Rule rule,
              const DayCounter& dayCounter, Real recoveryRate, const Handle<YieldTermStructure>& discountCurve,
              const Date& startDate = Date(), bool settlesAccrual = true, bool paysAtDefaultTime = true);
    CdsHelper(Rate quote, const Period& tenor,
              Integer settlementDays, // ISDA: 1
              const Calendar& calendar,
              Frequency frequency,                     // ISDA: Quarterly
              BusinessDayConvention paymentConvention, // ISDA: Following
              DateGeneration::Rule rule,               // ISDA: CDS
              const DayCounter& dayCounter,            // ISDA A360
              Real recoveryRate, const Handle<YieldTermStructure>& discountCurve, const Date& startDate = Date(),
              bool settlesAccrual = true, bool paysAtDefaultTime = true);
    void setTermStructure(DefaultProbabilityTermStructure*);
    boost::shared_ptr<QuantExt::CreditDefaultSwap> swap() const { return swap_; }

protected:
    void update();
    void initializeDates();
    virtual void resetEngine() = 0;
    Period tenor_;
    Integer settlementDays_;
    Calendar calendar_;
    Frequency frequency_;
    BusinessDayConvention paymentConvention_;
    DateGeneration::Rule rule_;
    DayCounter dayCounter_;
    Real recoveryRate_;
    Handle<YieldTermStructure> discountCurve_;
    bool settlesAccrual_;
    bool paysAtDefaultTime_;

    Schedule schedule_;
    boost::shared_ptr<QuantExt::CreditDefaultSwap> swap_;
    RelinkableHandle<DefaultProbabilityTermStructure> probability_;
    //! protection effective date.
    Date protectionStart_, startDate_;
};

//! Spread-quoted CDS hazard rate bootstrap helper.
//!  \ingroup termstructures
class SpreadCdsHelper : public CdsHelper {
public:
    SpreadCdsHelper(const Handle<Quote>& runningSpread, const Period& tenor, Integer settlementDays,
                    const Calendar& calendar, Frequency frequency, BusinessDayConvention paymentConvention,
                    DateGeneration::Rule rule, const DayCounter& dayCounter, Real recoveryRate,
                    const Handle<YieldTermStructure>& discountCurve, const Date& startDate = Date(),
                    bool settlesAccrual = true, bool paysAtDefaultTime = true);

    SpreadCdsHelper(Rate runningSpread, const Period& tenor, Integer settlementDays, const Calendar& calendar,
                    Frequency frequency, BusinessDayConvention paymentConvention, DateGeneration::Rule rule,
                    const DayCounter& dayCounter, Real recoveryRate, const Handle<YieldTermStructure>& discountCurve,
                    const Date& startDate = Date(), bool settlesAccrual = true, bool paysAtDefaultTime = true);
    Real impliedQuote() const;

private:
    void resetEngine();
};

//! Upfront-quoted CDS hazard rate bootstrap helper.
//!  \ingroup termstructures
class UpfrontCdsHelper : public CdsHelper {
public:
    /*! \note the upfront must be quoted in fractional units. */
    UpfrontCdsHelper(const Handle<Quote>& upfront, Rate runningSpread, const Period& tenor, Integer settlementDays,
                     const Calendar& calendar, Frequency frequency, BusinessDayConvention paymentConvention,
                     DateGeneration::Rule rule, const DayCounter& dayCounter, Real recoveryRate,
                     const Handle<YieldTermStructure>& discountCurve, const Date& startDate = Date(),
                     Natural upfrontSettlementDays = 0, bool settlesAccrual = true, bool paysAtDefaultTime = true);

    /*! \note the upfront must be quoted in fractional units. */
    UpfrontCdsHelper(Rate upfront, Rate runningSpread, const Period& tenor, Integer settlementDays,
                     const Calendar& calendar, Frequency frequency, BusinessDayConvention paymentConvention,
                     DateGeneration::Rule rule, const DayCounter& dayCounter, Real recoveryRate,
                     const Handle<YieldTermStructure>& discountCurve, const Date& startDate = Date(),
                     Natural upfrontSettlementDays = 0, bool settlesAccrual = true, bool paysAtDefaultTime = true);
    Real impliedQuote() const;
    void initializeDates();

private:
    Natural upfrontSettlementDays_;
    Date upfrontDate_;
    Rate runningSpread_;
    void resetEngine();
};
} // namespace QuantExt

#endif
