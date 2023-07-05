# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainClock
#
# Processing of menu Main -> Clock
# Show and change the values for time, date and timezone
#
{
  # loop until the user wants to get back to main menu
  local quit=$FALSE
  local selection
  local retUnused
  while [ "$quit" = "$FALSE" ]; do

    # read the values new in every loop, because most of them are addicted to the others
    ShowEvaluateDataWindow "Clock Configuration"

    local timeLocal=`./get_clock_data time-local`
    local timeUtc=`./get_clock_data time-utc`
    local date=`./get_clock_data date-local`
    local displayMode=`./get_clock_data display-mode`
    local timezone=`./get_clock_data timezone-string`
    local tzString=`./get_clock_data tz-string`

    if [[ "$timezone" == "" ]]; then
        timezone="unknown"
    fi

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Clock Configuration" \
              "0. Back to Main Menu" \
              "1. Date on device (local)..........$date" \
              "2. Time on device (local)............$timeLocal" \
              "3. Time on device (UTC)..............$timeUtc" \
              "4. Clock Display Mode..........$displayMode" \
              "5. Timezone............$timezone" \
              "6. TZ-String...........$tzString"

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # date was selected -> show input-window to get the new date
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Date" "Enter new Date:" 10 $date 2> temp
          local newDate=`cat temp`
          rm temp

          # if a new value was given, call the config-script and show possible error
          if [ -n "$newDate" ] && [ "$newDate" != "$date" ]; then
            ShowProcessingDataWindow "Clock Configuration"
            ./config_clock date=$newDate type=local
            ShowLastError
          fi
          ;;

      2)  # time local was selected -> show input-window to get the new time
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Time on Device Local" "Enter new Time:" 11 "$timeLocal" 2> temp
          local newTime=`cat temp`
          rm temp

          # if a new value was given, call the config-script and show possible error
          if [ -n "$newTime" ] && [ "$newTime" != "$timeLocal" ]; then
            ShowProcessingDataWindow "Clock Configuration"
            newTime=`ReplaceSpaces $newTime`
            ./config_clock type=local time=$newTime 
            ShowLastError
          fi
          ;;

      3)  # time UTC was selected -> show input-window to get the new time
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Time on Device UTC" "Enter new Time:" 11 "$timeUtc" 2> temp
          local newTime=`cat temp`
          rm temp

          # if a new value was given, call the config-script and show possible error
          if [ -n "$newTime" ] && [ "$newTime" != "$timeUtc" ] ; then
            ShowProcessingDataWindow "Clock Configuration"
            newTime=`ReplaceSpaces  $newTime`
            ./config_clock type=utc time=$newTime  
            ShowLastError
          fi
          ;;
      
      4)  # clock-display-mode (12-hour or 24-hour) was selected -> show selection-menu to get the new value
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Clock Dislplay Mode ($displayMode)" \
                    "0. Back to Clock Configuration Menu" \
                    "1. 24 hours" \
                    "2. 12 hours"

          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1)  local newMode="24-hour-format";;
            2)  local newMode="12-hour-format";;
            *)  local newMode="";;
          esac

          # if a new value for clock-display-mode was selected - change it
          if [ -n "$newMode" ] && [ "$displayMode" != "$newMode" ]; then
            ShowProcessingDataWindow "Clock Configuration"
            ./config_clock_display_mode display-mode=$newMode 
            ShowLastError
          fi
          ;;

      5)  # timezone was selected -> show selection-menu with the possible timezones
          ShowEvaluateDataWindow "Change Timezone"

          ifsBackup=$IFS
          IFS=$'\t'

          # show menu
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "To change Timezone please use Web-Based-Management (WBM)" \
                    "0. Back to Clock Configuration Menu" \
                    $menuString 
          IFS=$ifsBackup
          ;;

      # TZ-string should be changed
      6)  # TZ-string was selected -> show input-window to get the new TZ-String
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change TZ-String" "Enter new TZ-String:" 66 $tzString 2> temp
          local newTzString=`cat temp`
          rm temp

          # if a new TZ-string was given - change it and show possible error
          if [ -n "$newTzString" ] && [ "$newTzString" != "$tzString" ]; then
            ShowProcessingDataWindow "Clock Configuration"
            newTzString=`ReplaceSpaces $newTzString`
            ./config_timezone tz-string=$newTzString  
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}
# end of MainClock

MainClock
