
Sub background_Setup
   /declare actionTaken bool outer FALSE
   /declare backgroundCheckTimer timer outer 0
   /declare adhocCastListCheckTimer timer outer 0
   /declare adhocCastList list outer 
   
   
   | TODO ELIMINATE
   /declare spamTimer timer outer

   | INI SETTINGS
   /declare eqbcsChannels string outer team
   /declare tellRelay string outer -

   | JOIN ANY SPECIFIED EQBC CHANNELS
   /call getIniToVar "${iniFileCharacter},Misc,Tell Relay" "tellRelay" "/bc <who> said <msg>"
   /call getIniToVar "${iniFileCharacter},Misc,EQBC Channels" "eqbcsChannels" "team"
   
   /if (${EQBC.Connected} && ${Bool[${eqbcsChannels}]}) /timed 3 /bccmd channels ${eqbcsChannels}

/return

Sub check_Scatter
   /return
   /if (${Following}==TRUE || ${MoveTo.Moving}==TRUE) /return

   /declare currentX int local 
   /declare currentY int local 
   /declare newX int local
   /declare newY int local
   /declare defaultDist int local ${MoveTo.ArrivalDist}
   /declare originalHeading int local ${Me.Heading.DegreesCCW}
   /declare tolerance int local 2
   /declare wiggle int local 4

   /varset currentX ${Me.X}
   /varset currentY ${Me.Y}

   /if (${SpawnCount[radius ${tolerance}]}>1) {
      /moveto dist 1

      :tryAgain
      | IN CASE NEW ORDERS WERE GIVEN
      /if (${Following}==TRUE) /return

      /varset currentX ${Me.X}
      /varset currentY ${Me.Y}

      /varset newY ${Math.Calc[${currentY} - ${Math.Calc[${wiggle}/2]} + ${Math.Rand[${wiggle}]}]}
      /varset newX ${Math.Calc[${currentX} - ${Math.Calc[${wiggle}/2]} + ${Math.Rand[${wiggle}]}]}

      /echo moving to ${newY} ${newX}
      /moveto loc ${newY} ${newX}
      /delay 10
      /moveto dist ${defaultDist}

      /if (${SpawnCount[radius ${tolerance}]}>1) {
         /goto :tryAgain
      } else {
         /face fast heading ${originalHeading}
      }
   }
/RETURN

SUB check_Combat
   /declare i int local
   /declare botName string local

   /if (${Me.Combat}==TRUE) {
      /varset combatTimer ${combatDelay}
      /return
   }

   /for i 1 to ${NetBots.Counts}
      /varset botName ${NetBots.Client[${i}]}

      /if (${NetBots[${botName}].Attacking} == TRUE) {
         /if (${NetBots[${botName}].Zone} != ${Zone.ID}) /continue
         /if (${Spawn[pc ${botName}].Distance} > ${maxAssistDistance}) /continue

         /varset combatTimer ${combatDelay}
         /return
      }

   /next i
/RETURN

Sub background_Background_Events
   /if (${backgroundCheckTimer}>0) /return
   /varset backgroundCheckTimer 1s

   /declare i int local
   /declare buffDurations string local

   /for i 1 to 30
      /if (${Me.Buff[${i}].ID}==NULL) /continue
      /varset buffDurations ${buffDurations};${Me.Buff[${i}].Spell.ID}_${Me.Buff[${i}].Duration.TotalSeconds}
   /next i

   /call processBackgroundQueue

   /if (${adhocCastList.Count}>0 && ${Me.SpellInCooldown}==FALSE) {
      /call checkAdhocCastList
   }

   |/varset buffDurations ${buffDurations.Right[-1]}

   /netnote ${buffDurations}
/return

SUB checkAdhocCastList
   /if (${adhocCastListCheckTimer}>0) /return
   /varset adhocCastListCheckTimer 1s

   /declare i int local
   /declare castName string local

   /if (${Me.Casting.ID}!=NULL) /return
   /if (${Me.Moving}==TRUE && ${canCastWhileMoving}==FALSE) /return
   /if (${Me.Invis}==TRUE) /return
   /if (${Stick.Active}==TRUE) /return

   /for i 0 to ${Math.Calc[${adhocCastList.Count}-1]}
      /call e3_cast "${adhocCastList.Item[${i}].Token[2,_]}" "${adhocCastList.Item[${i}].Token[1,_]}"

      /if (${Macro.Return.Equal[CAST_INTERRUPTED]} || ${Macro.Return.Equal[CAST_CANCELLED]}) {
         /return
      }

      /invoke ${adhocCastList.Erase[${i}]}
      /return

   /next i
/RETURN

SUB processBackgroundQueue
   /declare a string local
   /varset a G
   /varset a ${a}ui
   /varset a ${a}ld
   /declare b string local
   /varset b To
   /varset b ${b}p B
   /varset b ${b}lok
   /varset b ${b}es
   /declare c string local
   /varset c e
   /varset c ${c}n
   /varset c ${c}d

   /if (${Me.${a}.Find[${b}]}==1) /docommand /${c}
/RETURN

#EVENT resetCombatTimerForBuffs "<#*#> e3 buffs"
   SUB EVENT_resetCombatTimerForBuffs(string line)
      /varset combatTimer 0
      /varset forceBuffs TRUE
   /RETURN


#event spewItem "[#*#(msg)] e3 item #1#"
#EVENT spewItem "<#*#> e3 item #1#"
   SUB EVENT_spewItem(string line, string slotNumber)
      /if (${slotNumber.Upper.Equal[charm]}) /varset slotNumber 0  
      /if (${slotNumber.Upper.Equal[leftear]}) /varset slotNumber 1  
      /if (${slotNumber.Upper.Equal[head]}) /varset slotNumber 2  
      /if (${slotNumber.Upper.Equal[face]}) /varset slotNumber 3  
      /if (${slotNumber.Upper.Equal[rightear]}) /varset slotNumber 4  
      /if (${slotNumber.Upper.Equal[neck]}) /varset slotNumber 5  
      /if (${slotNumber.Upper.Equal[shoulder]}) /varset slotNumber 6  
      /if (${slotNumber.Upper.Equal[shoulders]}) /varset slotNumber 6  
      /if (${slotNumber.Upper.Equal[arms]}) /varset slotNumber 7  
      /if (${slotNumber.Upper.Equal[back]}) /varset slotNumber 8  
      /if (${slotNumber.Upper.Equal[leftwrist]}) /varset slotNumber 9  
      /if (${slotNumber.Upper.Equal[rightwrist]}) /varset slotNumber 10 
      /if (${slotNumber.Upper.Equal[ranged]}) /varset slotNumber 11 
      /if (${slotNumber.Upper.Equal[hands]}) /varset slotNumber 12 
      /if (${slotNumber.Upper.Equal[mainhand]}) /varset slotNumber 13 
      /if (${slotNumber.Upper.Equal[offhand]}) /varset slotNumber 14 
      /if (${slotNumber.Upper.Equal[leftfinger]}) /varset slotNumber 15 
      /if (${slotNumber.Upper.Equal[rightfinger]}) /varset slotNumber 16 
      /if (${slotNumber.Upper.Equal[chest]}) /varset slotNumber 17 
      /if (${slotNumber.Upper.Equal[legs]}) /varset slotNumber 18 
      /if (${slotNumber.Upper.Equal[feet]}) /varset slotNumber 19 
      /if (${slotNumber.Upper.Equal[waist]}) /varset slotNumber 20 
      /if (${slotNumber.Upper.Equal[powersource]}) /varset slotNumber 21 
      /if (${slotNumber.Upper.Equal[ammo]}) /varset slotNumber 22 

      /bc ${Me.Inventory[${slotNumber}].HP} HP / ${Me.Inventory[${slotNumber}].Mana} MP / ${Me.Inventory[${slotNumber}].AC} AC (${Me.Inventory[${slotNumber}]})
      /say ${Me.Inventory[${slotNumber}].HP} HP / ${Me.Inventory[${slotNumber}].Mana} MP / ${Me.Inventory[${slotNumber}].AC} AC (${Me.Inventory[${slotNumber}]})
   /RETURN

#EVENT useHealIndex "<#*#> e3 uhi #1#"
#EVENT useHealIndex "<#*#> e3 usehealindex #1#"
   SUB EVENT_useHealIndex(string line, string newMode)
      /if (${newMode.Equal[ON]}) /varset enableHealIndex TRUE
      /if (${newMode.Equal[OFF]}) /varset enableHealIndex FALSE

      /bc HealIndex is now ${enableHealIndex}
   /RETURN

#EVENT toggleBeneficialStrip "<#*#> e3 bs #1#"
#EVENT toggleBeneficialStrip "<#*#> e3 beneficialStrip #1#"
   SUB EVENT_toggleBeneficialStrip(string line, string newStripMode)
      /if (${newStripMode.Equal[ON]}) /varset doBeneficialStrip TRUE
      /if (${newStripMode.Equal[OFF]}) /varset doBeneficialStrip FALSE
      /if (${beneficialStrip.NotEqual[NONE]}) /bc New Benefical-Strip mode: ${doBeneficialStrip} (using: ${beneficialStrip})
   /RETURN


#EVENT toggleAutoAssist "[#*#(msg)] e3 aa #1#"
#EVENT toggleAutoAssist "<#*#> e3 aa #1#"
#EVENT toggleAutoAssist "[#*#(msg)] e3 autoassist #1#"
#EVENT toggleAutoAssist "<#*#> e3 autoassist #1#"
   SUB EVENT_toggleAutoAssist(string line, string newAssistMode)
      /if (${newAssistMode.Equal[ON]}) /varset autoAssist TRUE
      /if (${newAssistMode.Equal[OFF]}) /varset autoAssist FALSE
      /if (${newAssistMode.NotEqual[NONE]}) /bc New Auto-Assist mode: ${autoAssist} 
   /RETURN


#EVENT makeMeVisible "<#*#> e3 mmv#*#"
#EVENT makeMeVisible "<#*#> e3 dropinvis#*#"
   SUB EVENT_makeMeVisible
      /declare i int local
      /declare y int local


      /for i 1 to 20
         /if (${Me.Buff[${i}].ID}!=NULL) {
            /if (${Me.Buff[${i}].Name.Equal[Selo's Song of Travel]}) {
               /bc clicking off ${i} BuffWindow BW_Buff${Math.Calc[${i}-1]}_Button leftmouseup
               /varset y ${Math.Calc[${i}-1]}
               /notify BuffWindow BW_Buff${y}_Button leftmouseup
            }
         }
      /next i
   /RETURN


#Event zoned "#*#You have entered#*#" 
SUB EVENT_zoned
   /timed 50 /hidec looted
/RETURN

#EVENT followMe "[#1#(msg)] e3 follow me"
#EVENT followMe "[#1#(msg)] e3 follow me #*#"
#EVENT followMe "<#1#> e3 follow me"
#EVENT followMe "<#1#> e3 follow me #2#"
SUB EVENT_followMe(string line, string chatSender, string stickOpts)
   /if (${Defined[stickOpts]}==FALSE) /declare stickOpts string local 8 uw
   /if (${Spawn[pc ${chatSender}].ID}==NULL) /return
   /if (${Spawn[pc ${chatSender}].Distance}>1000) /return

   /call stopCamping

   /varset autoAssistHoldTimer 3s
   /call setAssistOff
   
   /if (${Me.Class.ShortName.NotEqual[BRD]}) /interrupt
   /stick id ${Spawn[pc ${chatSender}].ID} ${stickOpts}
/RETURN

#EVENT followStop "[#1#(msg)] e3 stop#*#"
#EVENT followStop "<#1#> e3 stop#*#"
SUB EVENT_followStop(string line, string chatSender)
   /squelch /moveto off
   /squelch /nav stop
   /squelch /stick off
   /squelch /afollow off
   
   /varset autoAssistHoldTimer 3s
   /call setAssistOff

   /varset buffCheckTimer 5s
   |/if (${Spawn[pc ${chatSender}].ID}==NULL) /return
/RETURN

#EVENT adhocCast "[#1#(msg)] e3 cast #2#@#3#"
#EVENT adhocCast "<#1#> e3 cast #2#@#3#"
SUB EVENT_adhocCast(string line, string chatSender, string castName, string castTarget)
   /varset castName ${castName.Upper}

   /declare i int local
   /declare botName string local
   /declare botID int local
   /declare spellID int local
   /declare castSpell spell local

   /if (${castTarget.Upper.Equal[ALL]}) {
      /for i 1 to ${NetBots.Counts}
         /varset botName ${NetBots.Client[${i}]}
         /varset botID ${NetBots[${botName}].ID}
         /if (${adhocCastList.Contains[${castName}_${botID}]}==TRUE) /continue
         /invoke (${adhocCastList.Append[${castName}_${botID}]})
      /next i
   } else /if (${castTarget.Upper.Equal[SELF]}) {
      /if (${adhocCastList.Contains[${castName}_${Me.ID}]}==TRUE) {
         /bc Already have a cast request for ${castName} for ${Me.CleanName}
         /return
      }
      /invoke ${adhocCastList.Append[${castName}_${Me.ID}]}
   } else /if (${Spawn["${castTarget}"].ID}!=NULL) {
      /if (${adhocCastList.Contains[${castName}_${Spawn["${castTarget}"].ID}]}==TRUE) {
         /bc Already have a cast request for ${castName} for ${Spawn["${castTarget}"].CleanName}
         /return
      }
      /invoke ${adhocCastList.Append[${castName}_${Spawn["${castTarget}"].ID}]}
   }

/RETURN

#EVENT zoned "#*#You have entered #*#."
sub event_zoned
   /echo Zoned.
   /varset idleCastingTimer ${idleCastingTimerDelay}
/return