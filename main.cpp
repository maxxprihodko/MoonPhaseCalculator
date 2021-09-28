#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <array>
 
constexpr const char* USAGE_MACRO = "\n\tusage: <month> <date> <year>";

struct month_info
{
  const std::int32_t days;
  const std::int32_t month_count;

  month_info(std::int32_t days, std::int32_t month_count)
    : days(days)
    , month_count(month_count)
  {  }
};

struct data 
{
  month_info month;
  std::int32_t day;
  std::int32_t year;

  data(month_info month, std::int32_t day, std::int32_t year)
    : month(month)
    , day(day)
    , year(year)
  {  }
};

std::unordered_map<std::string, month_info> month_map
{
  { "january", { 31, 1 } },
  { "february", { 28, 2 } },
  { "march", { 31, 3 } },
  { "april", { 30, 4 } },
  { "may", { 31, 5 } },
  { "june", { 30, 6 } },
  { "july", { 31, 7 } },
  { "august", { 31, 8 } },
  { "september", { 30, 9 } },
  { "october", { 31, 10 } },
  { "november", { 30, 11 } },
  { "december", { 31, 12 } }
};

std::array<const std::int32_t, 12> month_array = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

std::array<const char* const, 12> month_names = 
{ 
  "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" 
};

void split(const std::string& str, std::vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delim)) 
        cont.push_back(token);
}

bool is_number(const std::string& str)
{
  std::size_t i = 0;

  for (const auto& c : str)
    if (std::isdigit(c)) ++i;

    return !str.empty() && i == str.size();
}

bool check_year(std::int32_t year)
{
  if (!year) return false;

  std::int32_t counter = 0;

  while (year /= 10)
    ++counter;
  return counter <= 5;
}

constexpr bool is_leap_year(std::int32_t year)
{
  return !(year % 4 && year % 100 && year % 400);
}

std::int32_t to_days(data dm)
{
  std::int32_t counter = 0;

  for (std::size_t i = 1; i < dm.year; ++i)
    counter += is_leap_year(i) ? 366 : 365;

  const auto is_leap = is_leap_year(dm.year);

  // Add all extra months check for leap years
  for (std::size_t i = 1; i < dm.month.month_count; ++i)
    counter += (i == 2 && is_leap) ? month_array[i - 1] + 1 : month_array[i - 1];

  for (std::size_t i = 1; i < dm.day; ++i)
    ++counter;
 
  if (dm.day > month_array[dm.month.month_count - 1])
  {
    if (is_leap && dm.day == 29 && dm.month.month_count == 2)
      return counter;

      std::cerr << "Date out of range for " << month_names[dm.month.month_count - 1] << ". Maximum amount of days are: " 
              << dm.month.days
              << USAGE_MACRO << '\n';
      exit(EXIT_FAILURE);
  }
  return counter;
}

std::int32_t to_int32(std::string str)
{
  if (is_number(str)) return std::stoi(str);
  
      std::cerr << "Incorrect input, expected '" << str << "' to be a number"
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
}

data parse_input(std::string str)
{
  std::vector<std::string> tokens;
  split(str, tokens);

  if (tokens.size() != 3)
  {
    std::cerr << "Incorrect input, expected 3 arguments not " << tokens.size() 
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
  }

  // Convert the string to lowercase 
  auto month_str = tokens[0];

  for (std::size_t i = 0; i < month_str.size(); ++i)
    month_str[i] = std::tolower(month_str[i]);

  // Search for the given month, if it doesn't exist we warn the user
  const auto& pair = month_map.find(month_str);

  if (pair == month_map.end())
  {
    std::cerr << "Non-existing month: " << '\'' << tokens[0] << '\'' 
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
  }

  try
  {
    const auto month = pair->second;
    const auto day = to_int32(tokens[1]);
    const auto year = to_int32(tokens[2]);

    const auto data_model = data(month, day, year);

    std::cout << "[DEBUG] Successfully parsed arguments." << '\n';
    return data_model;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Unexpected error while parsing arguments: " << e.what() 
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
  }
}

std::pair<std::int32_t, bool> calculate_days(data dm)
{
  /* 
    WARNING! CHANGE THIS DATE TO THE NEAREST FULL MOON
    (for me it will be in 8 days) 
  */

  // JANUARY 28 2021
  const auto full_moon = data({ 31, 1 }, 28, 2021);

  if (!check_year(dm.year))
  {
    std::cerr << "Invalid year, it must be greater than 0 and less than 99999, you said: '" << dm.year << '\'' 
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
  }

  if (dm.day > 31)
  {
    std::cerr << "Date out of range, maximum date in a month is 31, you said: '" << dm.day << '\''
              << USAGE_MACRO << '\n';
    exit(EXIT_FAILURE);
  }

  // Check if we are looking for a date in the past or the future
  const auto first = to_days(dm);
  const auto second = to_days(full_moon);
  
  const auto val = first > second ? first - second : second - first;
  
  return { val, first > second };
}

std::string to_phase(std::int32_t days, bool to_future)
{
  double lunations = days / 29.5306;
  std::cout << "[DEBUG] Number of lunations: " << lunations << '\n';

  const double decimal = std::abs(static_cast<const std::int32_t>(lunations) - lunations);
  
  const auto fixed_decimal = to_future ? decimal : 1.0 - decimal;

  if ((fixed_decimal >= 0.97 && fixed_decimal <= 1.0) || (fixed_decimal <= 0.03 && fixed_decimal >= 0.0))
    return "Full Moon";
  
  else if (fixed_decimal > 0.03 && fixed_decimal < 0.22)
     return "Waning Gibbous";
  
  else if (fixed_decimal >= 0.22 && fixed_decimal <= 0.28)
    return "Third Quarter";
  
  else if (fixed_decimal > 0.28 && fixed_decimal < 0.47)  
    return "Waning Crescent";
  
  else if (fixed_decimal >= 0.47 && fixed_decimal <= 0.53)
    return "New Moon";
  
  else if (fixed_decimal > 0.53 && fixed_decimal < 0.72) 
    return "Waxing Crescent";
  
  else if (fixed_decimal >= 0.72 && fixed_decimal <= 0.78) 
    return "First Quarter";
  
  else if (fixed_decimal > 0.78 && fixed_decimal < 0.97) 
    return "Waxing Gibbous";

  std::cerr << "Unknown moon phase, the decimal was: " << fixed_decimal << '\''
            << USAGE_MACRO << '\n';
  exit(EXIT_FAILURE);
}

int main() 
{
  while (true)
  {
    std::cout << "Your date: ";
    std::string input;
    std::getline(std::cin, input);
    std::cout << '\n';

    const auto dm = parse_input(input);
    const auto ret = calculate_days(dm);
    
    std::cout << "[DEBUG] Number of days: " << ret.first << '\n';
    
    const auto a = to_phase(ret.first, ret.second);
    std::cout << "[DEBUG] Moon Phase: " << a << "\n\n";
  }
}
