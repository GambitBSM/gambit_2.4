// ====================================================================
// This file is part of FlexibleSUSY.
//
// FlexibleSUSY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// FlexibleSUSY is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FlexibleSUSY.  If not, see
// <http://www.gnu.org/licenses/>.
// ====================================================================

// File generated at Wed 25 Oct 2017 18:18:42

/**
 * @file HSSUSY_two_scale_model.hpp
 * @brief contains class for model with routines needed to solve boundary
 *        value problem using the two_scale solver by solving EWSB
 *        and determine the pole masses and mixings
 *
 * This file was generated at Wed 25 Oct 2017 18:18:42 with FlexibleSUSY
 * 2.0.1 (git commit: unknown) and SARAH 4.11.0 .
 */

#ifndef HSSUSY_TWO_SCALE_H
#define HSSUSY_TWO_SCALE_H

#include "HSSUSY_model.hpp"
#include "HSSUSY_mass_eigenstates.hpp"

#include "model.hpp"

namespace flexiblesusy {

class Two_scale;
/**
 * @class HSSUSY<Two_scale>
 * @brief model class with routines for determining masses and mixings and EWSB
 */
template<>
class HSSUSY<Two_scale> : public Model, public HSSUSY_mass_eigenstates {
public:
   explicit HSSUSY(const HSSUSY_input_parameters& input_ = HSSUSY_input_parameters());
   HSSUSY(const HSSUSY&) = default;
   HSSUSY(HSSUSY&&) = default;
   virtual ~HSSUSY() = default;
   HSSUSY& operator=(const HSSUSY&) = default;
   HSSUSY& operator=(HSSUSY&&) = default;

   // interface functions
   virtual void calculate_spectrum() override;
   virtual void clear_problems() override;
   virtual std::string name() const override;
   virtual void run_to(double scale, double eps = -1.0) override;
   virtual void print(std::ostream& out = std::cerr) const override;
   virtual void set_precision(double) override;
};

std::ostream& operator<<(std::ostream&, const HSSUSY<Two_scale>&);

} // namespace flexiblesusy

#endif
