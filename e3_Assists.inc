| MANAGES MELEE ASSISTING, DEBUFFS, NUKES

SUB assist_CharacterSettings(bool backFillOnly)   
   /call WriteToIni "${iniFileCharacter},Assist,Auto-Assist" YES ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AutoAssist Percent" 99 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,Stick Point" BEHIND ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,Melee" YES ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AuotFire" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,Melee Distance" 8 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,Slay Index" 99 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,SlayReferencePoint" ${Me.CleanName} ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,BeneficialStrip" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,Tank" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,DontTakeAggroFrom" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,OnSlayAssistCommand" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,MaxAssistDistance" 1000 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,IgnoreAssistCalls" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,MaintainAssistLOS" YES ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AcceptCharmCommands" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,CharmSpellName" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,CharmDebuffs" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,CharmBreakCommand" NONE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AutoMez" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AutoMezSpell" ENTRANCE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AutoMezSpawnSearch" "radius 50 los range 1 10" ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Assist,AutoMezTimerMarginSeconds" 10 ${backFillOnly}
/RETURN

Sub assist_Setup
   /declare burnTimer timer outer 0
   /declare burnsActive bool outer FALSE
   /declare burnCheckTimer timer outer 0

   /declare combatTimer timer outer
   /declare combatDelay int outer 70
   /declare assistQueue string outer 
   /declare autoAbilitiesCheckTimer timer outer
   /declare assistTarget int outer 0
   /declare checkAssistSpellsTimer timer outer 0
   /declare allowControl bool outer FALSE
   /declare autoAssistHoldTimer timer outer
   /declare assistBeganInZoneID int outer 0
   /declare slayMode bool outer FALSE
   /declare slayProxIDs list outer
   /declare slayIndex int outer 1
   /declare slayReferencePoint string outer
   /declare checkSlayLockoutTimer timer outer 0
   /declare slayLockout bool outer FALSE
   /declare assistTime timer outer
   /declare assistStickPoint string outer BEHIND
   /declare dontTakeAggroFrom string outer
   /declare meleeAssist bool outer TRUE
   /declare auotFireAssist bool outer FALSE
   /declare autoFireMeleeMode bool outer FALSE
   /declare autoFireTankMode bool outer FALSE
   /declare assistStickDistance int outer 8
   /declare autoAssist bool outer FALSE
   /declare autoAssistPercent int outer 99
   /declare beneficialStrip string outer
   /declare doBeneficialStrip bool outer TRUE
   /declare debuffTargetIDs string outer
   /declare checkDebuffLockoutTimer timer outer
   /declare tankMode bool outer FALSE
   /declare tankCheckTimer timer outer
   /declare combatRestickTimer timer outer 0
   /declare maxAssistDistance int outer 1000
   /declare onSlayAssistCommand string outer -
   /declare ignoreAssistCalls bool outer FALSE
   /declare maintainAssistLOS bool outer TRUE
   /declare autoDisarm bool outer FALSE
   /declare meleeRetreat bool outer FALSE
   /declare meleeRetreatPercent bool outer 30
   /declare meleeRetreatResumePercent bool outer 70
   /declare meleeRetreatLocXYZ string outer -
   /declare amMeleeRetreating bool outer FALSE
   /declare doCharm bool outer FALSE
   /declare currentCharmSpawnID int outer 0
   /declare charmSpellName string outer -
   /declare charmDebuffSpellNames string outer
   /declare charmBreakCommand string outer NONE
   /declare autoMez bool outer FALSE
   /declare autoMezList list outer
   /declare autoMezSpell string outer NONE
   /declare autoMezSpawnSearch string outer NONE
   /declare autoMezTimerMarginSeconds string outer 10
   /declare autoMezCheckTimer timer outer 0
   /declare tauntSuccessList list outer 

   /declare joustEnabled bool outer FALSE
   /declare joustInTrigger string outer -
   /declare joustTime string outer 
   /declare joustTimer timer outer
   /declare joustDestYXZ string outer
   /declare joustWaitAtSafeSpot bool outer FALSE

   /declare campCheckTimer timer outer 
   /declare campEnabled bool outer FALSE
   /declare campX int outer
   /declare campY int outer   
   /declare campZ int outer

   /declare trapNames string outer a dark entity,a shimmering orb,a formless terror,a low hum,a singularity,a wardstone,a null field,an immobilizer,an explosion,a tanglefoot trap,a blast of heat

   /call getIniToVar "${iniFileCharacter},Assist,Slay Index" slayIndex 99
   /call getIniToVar "${iniFileCharacter},Assist,SlayReferencePoint" slayReferencePoint ${Me.CleanName}
   /call getIniToVar "${iniFileCharacter},Assist,Stick Point" assistStickPoint BEHIND
   /call getIniToVar "${iniFileCharacter},Assist,Melee" meleeAssist YES
   /call getIniToVar "${iniFileCharacter},Assist,Melee Distance" assistStickDistance 8
   /call getIniToVar "${iniFileCharacter},Assist,AutoFire" auotFireAssist NO
   /call getIniToVar "${iniFileCharacter},Assist,Auto-Assist" autoAssist FALSE
   /call getIniToVar "${iniFileCharacter},Assist,AutoAssist Percent" autoAssistPercent 99
   /call getIniToVar "${iniFileCharacter},Assist,BeneficialStrip" beneficialStrip NONE
   /call getIniToVar "${iniFileCharacter},Assist,Tank" tankMode FALSE
   /call getIniToVar "${iniFileCharacter},Assist,DontTakeAggroFrom" dontTakeAggroFrom NONE
   /call getIniToVar "${iniFileCharacter},Assist,OnSlayAssistCommand" onSlayAssistCommand NONE
   /call getIniToVar "${iniFileCharacter},Assist,MaxAssistDistance" maxAssistDistance 1000
   /call getIniToVar "${iniFileCharacter},Assist,IgnoreAssistCalls" ignoreAssistCalls FALSE
   /call getIniToVar "${iniFileCharacter},Assist,MaintainAssistLOS" maintainAssistLOS TRUE
   /call getIniToVar "${iniFileCharacter},Assist,AutoDisarm" autoDisarm FALSE
   /call getIniToVar "${iniFileCharacter},Assist,MeleeRetreat" meleeRetreat FALSE
   /call getIniToVar "${iniFileCharacter},Assist,MeleeRetreatPercent" meleeRetreatPercent 30
   /call getIniToVar "${iniFileCharacter},Assist,MeleeRetreatResumePercent" meleeRetreatResumePercent 70
   /call getIniToVar "${iniFileCharacter},Assist,AcceptCharmCommands" doCharm FALSE
   /call getIniToVar "${iniFileCharacter},Assist,CharmSpellName" charmSpellName FALSE
   /call getIniToVar "${iniFileCharacter},Assist,CharmDebuffs" charmDebuffSpellNames NONE
   /call getIniToVar "${iniFileCharacter},Assist,CharmBreakCommand" charmBreakCommand NONE
   /call getIniToVar "${iniFileCharacter},Assist,AutoMez" autoMez FALSE
   /call getIniToVar "${iniFileCharacter},Assist,AutoMezSpawnSearch" autoMezSpawnSearch "radius 50 los range 40 55"
   /call getIniToVar "${iniFileCharacter},Assist,AutoMezSpell" autoMezSpell NONE
   /call getIniToVar "${iniFileCharacter},Assist,AutoMezTimerMarginSeconds" autoMezTimerMarginSeconds 10

   /varset slayIndex ${Math.Calc[${slayIndex}-1]}
   
   /if (${tankMode}==TRUE) /varset assistStickPoint

   /if (${auotFireAssist}==TRUE && ${meleeAssist}==TRUE)  {
      /if (${meleeAssist}==TRUE) /varset autoFireMeleeMode TRUE
      /if (${tankMode}==TRUE) /varset autoFireTankMode TRUE
      /varset meleeAssist FALSE
      /varset tankMode FALSE
      /bc >>> AutoFire is enabled: Disablign Melee/Tank. Use "e3 toggle autofire [ON|OFF]" to toggle.<<<
   }

/return

SUB stickingToPlayer 
   /if (${Stick.Active}==TRUE) {
      /if (${Spawn[id ${Stick.StickTarget}].Type.Equal[PC]}) /return TRUE
      /return FALSE
   } else {
      /return FALSE
   }
/RETURN

Sub assist_Background_Events
   /if (${Me.Dead}==TRUE) {
      /if (${assistTarget}!=0) /call setAssistOff
      /return
   }

   /if (${joustEnabled}==TRUE) {
      /if (${joustWaitAtSafeSpot}==FALSE && ${joustTimer}==0) {
         /varset joustWaitAtSafeSpot TRUE
      }

      /if (${joustWaitAtSafeSpot}==TRUE) {
         /if (${Stick.Active}==TRUE) /stick off
         /if (${Me.Combat}==TRUE) /attack off
         /if (${Math.Distance[${joustDestYXZ}]} > 5) {
            /if (${Nav.Active}==FALSE) {
               /nav locyxz ${joustDestYXZ}
               /delay 5
            }
         }
         /return
      } 
   }

   /if (${autoDisarm}==TRUE) {
      /call checkAutoDisarm
   }

   | CHECK TANK ACTIONS
   /if (${tankMode}==TRUE && ${tankCheckTimer}==0) {
      /call checkTauntSuccessList
      /call checkTankStatus
      /varset tankCheckTimer 5
   }

   /if (${burnsActive}==TRUE && ${burnCheckTimer}==0) {
      /call checkBurns
      /varset burnCheckTimer 1s
   }

   /if (${campEnabled}==TRUE) /call checkCamp
   /if (${autoAssistHoldTimer}>0) /return
   /if (${autoAssist}==TRUE && ${assistTarget}==0) /call check_AutoAssist
   /if (${doCharm}==TRUE) /call checkCharm
   /if (${autoMez}==TRUE) /call checkAutoMez
   /if (${meleeRetreat}==TRUE && ${assistTarget}!=0) /call checkMeleeRetreat
   /if (${slayMode}==TRUE && ${meleeAssist}==TRUE) /call check_slay
   /if (${assistTarget}!=0) /call check_AssistStatus
   /if (${assistTarget}==0 && ${assistQueue.Length}>0) /call checkAssistQueue
   /if (${checkAssistSpellsTimer}==0 && ${Defined[iniAssistSpells]}==TRUE) /call checkAssistSpells iniAssistSpells
   /if (${canDoAutoAbilities}==TRUE) /call check_autoAbilities
   /if (${canDoDebuffs}==TRUE && ${Me.Moving}==FALSE && ${debuffTargetIDs.Length}>0) /call checkDebuffs
   /if (${beneficialStrip.NotEqual[NONE]} && ${doBeneficialStrip}==TRUE) /call check_Dispell
   /if (${auotFireAssist}==TRUE && ${assistTarget}!=0) /call checkAutoFireAssist
/return

SUB checkAutoFireAssist
   /declare tgt spawn local
   /vardata tgt Spawn[id ${assistTarget}]

   /declare minRange int local 30

   /if (${Me.TargetOfTarget.ID}==${Me.ID}) /return
   
   /if (${tgt.Distance}<${minRange}) {
      /bc >>> Target too close for autofire, backing up <<<
      /keypress back hold
      /timed 2 /keypress back release
      /delay 3
      /return
   }

   /if (${Me.AutoFire}==FALSE) {
      /autofire
   }
/RETURN

SUB checkCamp 
   /if (${campCheckTimer}>0) /RETURN
   /varset campCheckTimer 5

   /if (${Stick.Active}==TRUE || ${Navigation.Active}==TRUE) /RETURN
   /if (${combatTimer}>0) /RETURN

   /if (${Math.Distance[${campY} ${campX} ${campZ}]}>20) {
      /if (${Navigation.MeshLoaded}==TRUE && ${Navigation.Active}==FALSE && ) {
         /bc Returning to camp.
         /nav locxyz ${campX} ${campY} ${campZ}
      } else {
         /moveto loc ${campY} ${campX}
      }
   }
   
/RETURN

SUB checkTauntSuccessList
   /if (${tauntSuccessList.Count}==0) /return

   /declare i int local 
   /declare t int local

   /for i ${Math.Calc[${tauntSuccessList.Count}-1]} downto 0
      /varset t ${tauntSuccessList.Item[${i}].Token[2,_]}

      /if (${Math.Calc[${MacroQuest.Running} - ${t} >5000]}) {
         /invoke ${tauntSuccessList.Erase[${i}]}
      }  

   /next i
/RETURN

SUB checkAutoMez
   /if (${autoMezCheckTimer}>0) /return
   /if (${Stick.Active}==TRUE) /return
   /if (${Me.Moving}==TRUE) /return
   /if (${Me.Invis}==TRUE) /return

   /varset autoMezCheckTimer 3

   /declare spawnsInRangeCount int local ${SpawnCount[${autoMezSpawnSearch}]}
   /declare i int local
   /declare xSpawn spawn local
   /declare targetID int local
   /declare xSpell spell local

   | SCAN AND ADD CANDIDATES
   /if (${spawnsInRangeCount}>0) {
      /for i 1 to ${spawnsInRangeCount}
         /vardata xSpawn NearestSpawn[${i},${autoMezSpawnSearch}]
         /if (${Spawn[id ${xSpawn.ID}].ID}==${assistTarget}) /continue
         /if (${Spawn[id ${xSpawn.ID}].Dead}==TRUE) /continue
         /if (${Spawn[id ${xSpawn.ID}].ID}==NULL) /continue

         /if (${autoMezList.Index[${xSpawn.ID}]}==-1) {
            /invoke ${autoMezList.Append[${xSpawn.ID}]}
         }
      /next i
   }


   | MAINTENANCE: CLEANUP ANYTHING THAT MIGHT BE DEAD OR GONE OR IS BEING ATTACKED BY A NETBOT
   /if (${autoMezList.Count}>0) {
      /for i ${Math.Calc[${autoMezList.Count}-1]} downto 0
         /if (${Spawn[id ${autoMezList.Item[${i}]}].Aggressive}==FALSE || ${autoMezList.Item[${i}]}==${assistTarget} || ${Spawn[id ${autoMezList.Item[${i}]}].Dead}==TRUE || ${Spawn[id ${autoMezList.Item[${i}]}].ID}==NULL || ${isNetBotAttackingID[${autoMezList.Item[${i}]}]}==TRUE) {
            /call wipeLockout mez_${autoMezList.Item[${i}]}
            /invoke ${autoMezList.Erase[${i}]}
            /continue
         }
      /next i
   }

   /if (${autoMezList.Count}==0) /return

   | PROCESS LIST AND MEZ 
   /for i 0 to ${Math.Calc[${autoMezList.Count}-1]}
      /varset targetID ${autoMezList.Item[${i}]}

      /if (${Spawn[id ${targetID}].Dead}) /continue

      /call fullTarget ${targetID}

      /if (${Target.Mezzed.ID}!=NULL) {
         /if (${Target.Mezzed.Duration.TotalSeconds} > ${autoMezTimerMarginSeconds}) {
            /call lockout mez_${targetID} ${Math.Calc[${Target.Mezzed.Duration.TotalSeconds}-${autoMezTimerMarginSeconds}]}s
            /continue
         }
      }

      /if (${Target.Mezzed.ID}==NULL || ${isLockoutExpired[mez_${targetID}]}==TRUE) {
         /call e3_getSpellByCastRequestName "${autoMezSpell}"
         /vardata xSpell ${Macro.Return}

         :recast
         /face fast id ${targetID}
         /call e3_cast ${targetID} "${autoMezSpell}" NETBOTATTACKING,TARGETISASSITTARGET

         /if (${Macro.Return.Equal[NOTREADY]} || ${Macro.Return.Equal[CAST_INTERRUPTED]} ) {
            /delay 2
            /goto :recast
         }

         /if (${Macro.Return.Equal[CAST_SUCCESS]}) {
            /call lockout mez_${targetID} ${Math.Calc[${xSpell.Duration.TotalSeconds}-${autoMezTimerMarginSeconds}]}s
            /echo ${xSpell}
            /echo ${xSpell.Duration.TotalSeconds}
            /echo ${autoMezTimerMarginSeconds}
            /echo /call lockout mez_${targetID} ${Math.Calc[${xSpell.Duration.TotalSeconds}-${autoMezTimerMarginSeconds}]}s
         }      
      }

   /next i
/RETURN

SUB checkCharm
   /if (${currentCharmSpawnID}==0) /return
   /if (${Me.Moving}==TRUE) /return
   /if (${Me.Invis}==TRUE) /return
   /if (${Me.Pet.ID}==${currentCharmSpawnID}) /return

   /if (${Spawn[id ${currentCharmSpawnID}].ID}==NULL) {
      /varset currentCharmSpawnID 0
      /return
   }

   /if (${Spawn[id ${currentCharmSpawnID}].Dead}==TRUE) {
      /varset currentCharmSpawnID 0
      /return
   }

   /declare i int local 1

   /tar id ${currentCharmSpawnID}
   /delay 10

   /if (${charmDebuffSpellNames.NotEqual[NONE]}) {
      /while (${Bool[${charmDebuffSpellNames.Token[${i},,]}]}!=FALSE) {
         /if (${Target.Buff[${charmDebuffSpellNames.Token[${i},,]}].ID}==NULL) {
            /bc Won't charm ${currentCharmSpawnID} because it's missing ${charmDebuffSpellNames.Token[${i},,]}...
            /call e3_cast ${currentCharmSpawnID} "${charmDebuffSpellNames.Token[${i},,]}"
         }
         /varset i ${Math.Calc[${i}+1]}
      }
   }

   /delay 3
   /call e3_Cast ${currentCharmSpawnID} "${charmSpellName}"
/return

SUB checkMeleeRetreat
   /if (${Me.PctHPs} >= ${meleeRetreatResumePercent} && ${amMeleeRetreating}==TRUE) {
      /varset amMeleeRetreating FALSE
      /return
   }

   /if (${Me.PctHPs} <= ${meleeRetreatPercent}) {
      /varset amMeleeRetreating TRUE
      /call setAssistOff
      /nav locxyz ${meleeRetreatLocXYZ}
   }
/RETURN

SUB checkAutoDisarm
   /if (${Me.AbilityReady[Sense Traps]}==TRUE) {
      /doability "sense traps"
      /delay 3
   }


   /if (${Me.AbilityReady[Disarm Traps]}==FALSE) /return
   /declare i int local 0
   /declare tName string local

   /for i 1 to ${Math.Calc[${trapNames.Count[,]}+1]}
      /varset tName ${trapNames.Token[${i},,]}
      /if (${Spawn[${tName}].ID}!=NULL) {
         /if (${Spawn[${tName}].Distance}<75) {
            /tar ${tName}
            /delay 2
            /doability "disarm traps"
            /RETURN
         }
      }
   /next i
/RETURN

SUB checkBurns 
   /if (${burnTimer}==0) {
      /bc Burn timer finished.
      /varset burnsActive FALSE
      /return
   }

   /if (${assistTarget}==0) /return

   /if (${iniBurns.Size}!=NULL) {
      /call checkAssistSpells iniBurns TRUE
   }  
/return

SUB checkTankStatus
   /if (${assistTarget}==0) /return

   /declare i int local 
   /declare entry string local

   |/if (${dontTakeAggroFrom.NotEqual[NONE]} && ${Me.SecondaryAggroPlayer.ID}!=NULL) {
   |   /call characterPassesAllowKeySeries ${Me.SecondaryAggroPlayer.CleanName.Upper} "${dontTakeAggroFrom.Upper}"
   |   /if (${Macro.Return}==TRUE) /return
   |}

   /if (${dontTakeAggroFrom.NotEqual[NONE]} && ${Me.TargetOfTarget.ID}!=NULL) {
      /call characterPassesAllowKeySeries ${Me.TargetOfTarget.CleanName.Upper} "${dontTakeAggroFrom.Upper}"
      /if (${Macro.Return}==TRUE) /return
   }

   /for i 1 to ${iniAggroAbilities.Size}
      /if (${iniAggroAbilities[${i},1].Equal[-]}) /return

      /if (${iniAggroAbilities[${i},${bNotToT}].NotEqual[-]}) {
         /if (${Me.TargetOfTarget.ID}==${Me.ID}) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bAmToT}].NotEqual[-]}) {
         /if (${Me.TargetOfTarget.ID}!=${Me.ID}) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bOnlyAfterTaunt}].NotEqual[-]}) {
         /if (${tauntSuccessListContains[${assistTarget}_]}==FALSE) {
            /goto :loopnexti
         }
      }

      /if (${iniAggroAbilities[${i},${bDoAbility}].NotEqual[-]}) {
         /if (${Me.AbilityReady[${iniAggroAbilities[${i},1]}]} == FALSE) /goto :loopnexti 
      }

      /if (${iniAggroAbilities[${i},${bSelfNotHighestAggro}].NotEqual[-]}) {
         /if (${Me.PctAggro}==100) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bCombatTimeOver}].NotEqual[-]}) {
         /if (${Math.Calc[(12000-${assistTime})/10]} < ${iniAggroAbilities[${i},${bCombatTimeOver}]}) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bSelfOverEnd}].NotEqual[-]}) {
         /if (${Me.PctEndurance} < ${iniAggroAbilities[${i},${bSelfOverEnd}]} ) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bNextHighestAggroOver}].NotEqual[-]}) {
         /if (${Me.PctAggro} != 100) /goto :loopnexti
         /if (${Me.SecondaryPctAggro} < ${iniAggroAbilities[${i},${bNextHighestAggroOver}]}) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bTargetUnderDistance}].NotEqual[-]}) {
         /if (${Target.Distance} > ${iniAggroAbilities[${i},${bTargetUnderDistance}]}) /goto :loopnexti
      }

      /if (${iniAggroAbilities[${i},${bRequireMobsInProximity}].NotEqual[-]}) {
         /if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 25]} < ${iniAggroAbilities[${i},${bRequireMobsInProximity}]}) /goto :loopnexti
      }


      /if (${iniAggroAbilities[${i},${bDoAbility}].NotEqual[-]}) {
         /doability "${iniAggroAbilities[${i},1]}"
         /delay 3
         /goto :loopnexti
      } else {
         /if (${Me.Moving}==TRUE) {
            /multiline ; /moveto stop ; /nav stop ; /stick off
            /delay 5
         }
         /call e3_Cast ${assistTarget} "${iniAggroAbilities[${i},1]}"
      }

      /if (${Macro.Return.Equal[CAST_SUCCESS]}) /return

      :loopnexti
   /next i
/RETURN

SUB check_slay
   /if (${Me.Invis}==TRUE) /RETURN
   /if (${slayLockout}==TRUE) /RETURN
   /if (${checkSlayLockoutTimer}>0) /RETURN
   /varset checkSlayLockoutTimer 10

   /if (${stickingToPlayer[]}==TRUE) /RETURN
   /if (${Navigation.Active}==TRUE) /RETURN

   /declare xSpawn spawn local
   /declare refX int local ${Spawn[${slayReferencePoint}].X}
   /declare refY int local ${Spawn[${slayReferencePoint}].Y}
   /declare refZ int local ${Spawn[${slayReferencePoint}].Z}
   /declare search string local npc los targetable range 1 60 radius 35 loc ${refX} ${refY}
   /declare xID int local
   /declare max int local ${SpawnCount[${search}]}
   /declare i int local

   /if (${slayIndex}>90) /return
   /if (${max}==0) /RETURN

   /for i 1 to ${max}
      /varset xID ${NearestSpawn[${i},${search}].ID}
      /if (${slayProxIDs.Index[${xID}]}==-1) {
         /invoke ${slayProxIDs.Append[${xID}]}
      }
   /next i

   /bc MSI: ${slayProxIDs.Item[${slayIndex}]}  (${slayIndex})  Lockout=${slayLockout}

   /if (${slayProxIDs.Item[${slayIndex}]}==FALSE) /return

   /bc SPIDs: ${slayProxIDs.Item[0]} ${slayProxIDs.Item[1]} ${slayProxIDs.Item[2]} ${slayProxIDs.Item[3]} ${slayProxIDs.Item[4]}

   /if (${slayLockout}==FALSE) {
      /bc Engaging SlayIndex[${slayIndex}] on${slayProxIDs.Item[${slayIndex}]}
      /call setAssistOn ${slayProxIDs.Item[${slayIndex}]}
      /varset slayLockout TRUE
      /if (${onSlayAssistCommand.NotEqual[NONE]}) /call e3command "${onSlayAssistCommand}"
   }
/RETURN

SUB setAssistOn(targetID)

   /if (${amMeleeRetreating}==TRUE) /return
   |/if (${autoAssistHoldTimer}>0) /return
   /if (${SpawnCount[id ${targetID}]}==0) /return
   |/if (${Spawn[id ${targetID}].Type.Equal[PC]}) /return

   /invoke ${slayProxIDs.Clear}
   /varset slayLockout FALSE
   /varset checkSlayLockoutTimer 15
   /varset assistTime 1200s
   /varset assistTarget ${targetID}

   /varset assistBeganInZoneID ${Zone.ID}
   /varset allowControl TRUE
   /face fast id ${targetID}
   /tar id ${targetID}
   /if (${Me.Pet.ID}) /timed 3 /pet attack

/RETURN

SUB check_AssistStatus

   | DID I CHANGE ZONES? 
   /if (${Zone.ID} != ${assistBeganInZoneID}) {
      /bc I'm no longer in the right zone. Stopping assist.
      /call setAssistOff
   }

   | NAVIGATING? DONT TRY TO DO ANYTHING 
   /if (${Navigation.Active}==TRUE) /return

   | SPAWN GO MISSING? STOP ASSIST
   /if (${SpawnCount[id ${assistTarget}]}==0) {
      /call setAssistOff
      /return
   }

   /declare assistSpawn spawn local
   /vardata assistSpawn Spawn[id ${assistTarget}]

   /if (${assistSpawn.Dead}) {
      /if (${assistQueue.Length}>0) {
         /call checkAssistQueue
      } else {
         /call setAssistOff
      }
      /RETURN
   }

   | stop assisting against pets belonging to players
   /if (${assistSpawn.Type.Equal[Pet]} && ${assistSpawn.Master.Type.Equal[PC]}) {
      /call setAssistOff
      /return
   }

   | Check range.
   /if (${Spawn[${assistTarget}].Distance} > ${maxAssistDistance}) {
      /echo [${Spawn[${assistTarget}].CleanName} - ${Spawn[${assistTarget}].ID}] is out of range,  holding assist functions.
      /RETURN
   }
   
   | Check feigning
   /if (${Me.Feigning} || ${allowControl}==FALSE) {
      /echo I am feigned or allowControl is true, holding assist functions. 
      /varset combatTimer ${combatDelay}
      /RETURN
   }

   | Check Target.

   /if (${meleeAssist}==TRUE) {
      /if (${Target.ID}!=${assistTarget}) /tar id ${assistTarget}
      /if (${Stick.StickTarget} != ${assistTarget} || ${Stick.Active}==FALSE) /stick ${assistStickDistance} id ${assistTarget} ${assistStickPoint} moveback uw
      /if (${Me.Combat}==FALSE) /attack on
   }

   /if (${Spawn[id ${assistTarget}].LineOfSight}==FALSE && ${maintainAssistLOS}==TRUE) {
      /if (${Spawn[id ${assistTarget}].ID}==NULL) {
         /nav stop
      } else {
         /nav id ${assistTarget}
      }
   }

   | TOO FAR? MOVE TO
   |/if (${assistSpawn.Distance} > ${assistStickDistance} && ${meleeAssist}==TRUE) {
   |   /bc too far..  ${assistSpawn.Distance}  /   ${assistStickDistance}
   |   /if (${Me.Moving}==FALSE) {
   |      /moveto loc ${assistSpawn.Y} ${assistSpawn.X}
   |      /return
   |   }
   |}

   | BEGIN ENGAGE

   /if (${meleeAssist}==TRUE) {
      /if (${combatRestickTimer}==0) {
         /varset combatRestickTimer 3s
         /squelch /stick ${assistStickDistance} id ${assistTarget} ${assistStickPoint} moveback uw  
      }
   }

   /varset combatTimer ${combatDelay}
/RETURN

SUB checkAssistQueue
   /declare id string local

   /if (${assistQueue.Length}>0) {
      /varset id ${assistQueue.Token[1,|]}

      /if (${Spawn[id ${id}].Dead}==TRUE || ${Spawn[id ${id}].ID}==NULL) {
      } else {
         /call setAssistOn ${id}
      }

      /varset assistQueue ${assistQueue.Replace[${id}|,]}
   }
/RETURN

SUB setAssistOff
   /attack off
   /target clear
   /if (${Stick.Active}==TRUE && ${Stick.StickTarget}==${assistTarget}) /squelch /stick off
   /if (${Me.Pet.ID}) /pet back off
   /if (${Me.AutoFire}) /autofire off

   /invoke ${slayProxIDs.Clear}
   /varset slayLockout FALSE
   /varset debuffTargetIDs
   /varset assistQueue
   /varset assistTime 0
   /varset assistTarget 0
   /varset assistBeganInZoneID 0
   /varset autoAssistHoldTimer 1s
/RETURN

| SEE IF I SHOULD AUTO-ASSIST SOMETHING THAT A FELLOW NETBOT IS ATTACKING
SUB check_AutoAssist
   /if (${autoAssist}==FALSE) /return
   /if (${autoAssistHoldTimer}>0) /return

   /declare i int local
   /declare netbotName string local
   /declare targetID int local 0

   /for i 1 to ${NetBots.Counts}
      /varset netbotName ${NetBots.Client[${i}]}

      /if (${NetBots[${netbotName}].InZone}==FALSE) /goto :loopnext

      | PHYSICALLY ATTACKING SOMETHNG?
      /if (${NetBots[${netbotName}].Attacking}==FALSE) /goto :loopnext
      /if (${NetBots[${netbotName}].TargetID}==0) /goto :loopnext
      /if (${Spawn[id ${NetBots[${netbotName}].TargetID}].Dead}==TRUE) /goto :loopnext


      /if (${Spawn[id ${NetBots[${netbotName}].TargetID}].Type.NotEqual[PC]} && ${NetBots[${netbotName}].TargetHP}<=${autoAssistPercent}) {
         /varset targetID ${NetBots[${netbotName}].TargetID}
      }

      :loopnext
   /next i
   :loopesc

   /if (${targetID}!=0) /call setAssistOn ${targetID}
/RETURN

SUB check_Dispell
   | FOR SOME REASON NOT TARGETTING ANYTHING?
   /if (${Target.ID}==NULL) /return
   /if (${assistTarget}==0) /return

   | NOTHING TO DISPELL?
   /if (${Target.Beneficial.ID}==NULL) /return

   /if (${isCastReady[${beneficialStrip}]}==TRUE) {
      /stick off
      |/delay 5
      /call e3_cast ${assistTarget} "${beneficialStrip}"
   } else {
      |/echo ${beneficialStrip} not ready.
   }
/RETURN

SUB checkAssistSpells(string processArray, bool stopAfterActionTaken)

   /varset checkAssistSpellsTimer 5
   /if (${assistTarget}==0) /return

   /if (${Me.Moving}==TRUE && ${canCastWhileMoving}==FALSE) /return

   /declare i int local
   /declare castResult string local
   /declare assistSpell spell local
   /declare assistSpellName string local
   /declare assistTimerName string local
   /declare assistTimerValue int local 0
   /declare maxResists int local 5
   /declare currentResistCount int local 0
   /declare cancelConditions string local

   /declare lifeDeficit int local
   /declare lifeRatio int local

   /for i 1 to ${${processArray}.Size}
      /varset assistSpellName ${${processArray}[${i},1]}
      /varset assistTimerValue 0
      /varset currentResistCount 0
      /varset maxResists 5
      /varset cancelConditions TARGETDEAD

      /if (${assistSpellName.Equal[-]}) /break

      /if (${${processArray}[${i},${bRequireCastIdle}].NotEqual[-]}) {
         /if (${idleCastingTimer}>0) /continue
      }

      /if (${${processArray}[${i},${bMaxResists}].NotEqual[-]}) {
         /varset maxResists ${${processArray}[${i},${bMaxResists}]}
      }

      /if (${${processArray}[${i},${bSelfOverMana}].NotEqual[-]}) {
         /if (${Me.PctMana} <= ${${processArray}[${i},${bSelfOverMana}]}) /continue
      }

      /if (${${processArray}[${i},${bSelfUnderMana}].NotEqual[-]}) {
         /if (${Me.PctMana} >= ${${processArray}[${i},${bSelfUnderMana}]}) /continue
      }

      /if (${${processArray}[${i},${bSelfUnderEnd}].NotEqual[-]}) {
         /if (${Me.PctEndurance} >= ${${processArray}[${i},${bSelfUnderEnd}]} ) /continue
      }
      /if (${${processArray}[${i},${bSelfOverEnd}].NotEqual[-]}) {
         /if (${Me.PctEndurance} < ${${processArray}[${i},${bSelfOverEnd}]} ) /continue
      }

      /if (${${processArray}[${i},${bTargetOverLevel}].NotEqual[-]}) {
         /if (${Spawn[id ${assistTarget}].Level} < ${${processArray}[${i},${bTargetOverLevel}]}) /continue
      }

      /if (${${processArray}[${i},${bTargetUnderLevel}].NotEqual[-]}) {
         /if (${Spawn[id ${assistTarget}].Level} >= ${${processArray}[${i},${bTargetUnderLevel}]}) /continue
      }

      /if (${${processArray}[${i},${bRequireBodyType}].NotEqual[-]}) {
         /if (${Select[${Spawn[id ${assistTarget}].Body.Name},${${processArray}[${i},${bRequireBodyType}]}]}==0) /continue
      }

      /if (${${processArray}[${i},${bTargetRace}].NotEqual[-]}) {
         /if (${Select[${Spawn[id ${assistTarget}].Race},${${processArray}[${i},${bTargetRace}]}]}==0) /continue
      }

      /if (${${processArray}[${i},${bRequireMobsInProximity}].NotEqual[-]}) {
         /if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 25]} < ${${processArray}[${i},${bRequireMobsInProximity}]}) /continue
      }

      /if (${${processArray}[${i},${bRequireMaxMobsInProximity}].NotEqual[-]}) {
         /if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 45]} > ${${processArray}[${i},${bRequireMaxMobsInProximity}]}) /continue
      }

      /if (${${processArray}[${i},${bRequireTargetClass}].NotEqual[-]}) {
         /if (${Select[${Spawn[id ${assistTarget}].Class.ShortName},${${processArray}[${i},${bRequireTargetClass}]}]}==0) /continue
      }

      /if (${${processArray}[${i},${bRequireNoActiveDisc}].NotEqual[-]}) {
         /if (${Me.ActiveDisc.ID}!=NULL) /continue
      }

      /if (${${processArray}[${i},${bTargetOverHP}].NotEqual[-]}) {
         /call fullTarget ${assistTarget}
         /if (${Target.PctHPs} < ${${processArray}[${i},${bTargetOverHP}]}) /continue
         /varset cancelConditions ${cancelConditions},TARGETUNDERHP:${${processArray}[${i},${bTargetOverHP}]}
      }

      /if (${${processArray}[${i},${bTargetUnderHP}].NotEqual[-]}) {
         /call fullTarget ${assistTarget}
         /if (${Target.PctHPs} >= ${${processArray}[${i},${bTargetUnderHP}]}) /continue
      }

      /if (${${processArray}[${i},${bTargetLifeManaRatio}].NotEqual[-]}) {
         /call fullTarget ${assistTarget}

         /varset lifeDeficit ${Math.Calc[100-${Target.PctHPs}]}
         /varset lifeRatio ${${processArray}[${i},${bTargetLifeManaRatio}]}
         /if (${Me.PctMana} < ${Math.Calc[100-(${lifeDeficit}/${lifeRatio})]}) {
            /continue
         }
      }


      | RESOLVE THE NUKE NAME REQUEST INTO A SPELL OBJECT
      /call e3_getSpellByCastRequestName "${assistSpellName}"

      /if (${Macro.Return.Equal[UNKNOWN]}) {
         /echo unknown spell: ${assistSpellName} 
         /continue
      }
      /vardata assistSpell ${Macro.Return}

      | BUILD TIMER NAME IN EVENT OF RECAST DELAY OR NOTAKEHOLD
      /varset assistTimerName detriTimer_${assistTarget}_${assistSpell.ID}


      | TIMER FOR THIS? SKIP...
      /if (${Defined[${assistTimerName}]}==TRUE) {
         /if (${${assistTimerName}}>0) /continue
         /deletevar ${assistTimerName}
      }

      :resistRetry

      /if (${${processArray}[${i},${bMemInGem}].NotEqual[-]}) {
         /if (${Me.Gem[${assistSpellName}]}==NULL) {
            /call memorizeSpellInGem "${assistSpellName}" ${${processArray}[${i},${bMemInGem}]}
         }
      }
      
      /call e3_cast ${assistTarget} "${assistSpellName}" "${cancelConditions}"

      /varset castResult ${Macro.Return}

      /if (${castResult.Equal[CAST_NOTAKEHOLD]}) {
         /declare ${assistTimerName} timer outer 120
      } else /if (${castResult.Equal[CAST_SUCCESS]}) {
         | handle medbreak pause
         /if (${${processArray}[${i},${bNoSitTimer}].NotEqual[-]}) {
            /call pauseMedBreak ${${processArray}[${i},${bNoSitTimer}]}
         }

         /if (${${processArray}[${i},${bRecastDelay}].NotEqual[-]} || ${assistSpell.Duration.TotalSeconds} > 0) {
            /declare ${assistTimerName} timer outer 0

            /if (${${processArray}[${i},${bRecastDelay}].NotEqual[-]}) {
               /varset ${assistTimerName} ${${processArray}[${i},${bRecastDelay}]}
            }

            /if (${assistSpell.Duration.TotalSeconds}>0) {
               /varset ${assistTimerName} ${Math.Calc[${${assistTimerName}} + ${assistSpell.Duration.TotalSeconds}]}s
            }
         }

         /varset actionTaken TRUE
         /if (${stopAfterActionTaken}==TRUE) /break
      } else /if (${castResult.Equal[CAST_RESIST]}) {
         /varset currentResistCount ${Math.Calc[${currentResistCount}+1]}
         /bc Cast Attempts: ${currentResistCount} of ${maxResists}
         /if (${currentResistCount}<${maxResists}) {
            /while (${isCastReady[${assistSpellName}]}==FALSE) {
               /delay 2
            }
            /goto :resistRetry
         }

         /if (${currentResistCount}==${maxResists}) {
            /bc My spell was resisted too much.  Giving up...
            /g Giving up casting ${assistSpellName} on ${assistTarget}, too many resists.
            /declare ${assistTimerName} timer outer 5m
         }
      } else /if (${castResult.Equal[CAST_IMMUNE]}) {
         /bc Immune.  Giving up...
         /g Giving up casting ${assistSpellName} on ${assistTarget}, target is immune.
         /call lockout ${assistTimerName} 5m
      }
   
   /next i
/RETURN

SUB checkDebuffs
   /declare targetEntry string local
   /declare targetID int local
   /declare targetZone int local
   /declare i int local 1

   /if (${checkDebuffLockoutTimer}>0) /return
   /varset checkDebuffLockoutTimer 3
   
   :loopStart
   |/while (1) {
      /varset targetEntry ${debuffTargetIDs.Token[${i},;]}
      
      /if (${targetEntry.Length}==NULL) /goto :loopesc

      /varset targetID ${targetEntry.Arg[1,_]}
      /varset targetZone ${targetEntry.Arg[2,_]}

      /if (${Zone.ID} != ${targetZone} || ${Spawn[id ${targetID}].Dead}==TRUE || ${SpawnCount[id ${targetID}]}==0) {
         /echo clearing debuffTargetID:  ${targetID}
         /varset debuffTargetIDs ${debuffTargetIDs.Replace[${targetEntry};,]}
         /goto :loopesc
      }

      /call processDebuffsForTargetID ${targetID}

      /varset i ${Math.Calc[${i}+1]}
      :loopNext
   }
   /goto :loopStart
   :loopesc
/RETURN

SUB check_autoAbilities
   /declare i int local
   /declare abilityName string local

   /if (${autoAbilitiesCheckTimer}>0) /return
   /varset autoAbilitiesCheckTimer 1

   /for i 1 to ${iniAutoAbilities.Size}
      /varset abilityName ${iniAutoAbilities[${i},1]}

      /if (${abilityName.Equal[-]}) /goto :loopesci

      /if (${iniAutoAbilities[${i},${bSelfUnderHP}].NotEqual[-]}) {
         /if (${Me.PctHPs} >= ${iniAutoAbilities[${i},${bSelfUnderHP}]}) /goto :loopnexti
      }

      /if (${iniAutoAbilities[${i},${bRequireAssistTarget}].NotEqual[-]}) {
         /if (${assistTarget}==0) /goto :loopnexti
      }

      /if (${iniAutoAbilities[${i},${bFreeze}].NotEqual[-]}) {
         /delay ${iniAutoAbilities[${i},${bFreeze}].NotEqual[-]}
      }

      /if (${iniAutoAbilities[${i},${bDoCommand}].NotEqual[-]}) {
         /call e3command "${iniAutoAbilities[${i},${bDoCommand}]}"
      }
      

      /if (${Me.AbilityReady[${abilityName}]}==TRUE) {
         /doability "${abilityName}"
      }

      :loopnexti
   /next i
   :loopesci
/RETURN

SUB clearLongTermSpellTimers(int targetID, string debuffArrayName)
   /declare timerName string local
   /declare castSpell spell local
   /declare castName string local
   /declare i int local 
   
   /for i 1 to ${${debuffArrayName}.Size}
      /varset castName ${${debuffArrayName}[${i}].Arg[1,|]}

      /call e3_getSpellByCastRequestName "${castName}"
      /if (${Macro.Return.Equal[UNKNOWN]}) /goto :loopend
      /vardata castSpell ${Macro.Return}

      /varset timerName detriTimer_${targetID}_${castSpell.ID}

      /if (${Defined[${timerName}]}==TRUE) {
         /bc Clearing long term spell timer ${timerName} (${castName} @ ${Spawn[id ${targetID}].Name})
         /deletevar ${timerName}
      }

      :loopend
   /next i
/RETURN


|----------------------------------------------------|
|- Casts debuffs and dots on various assist targets -|
|----------------------------------------------------|
SUB processDebuffsForTargetID(int targetID)
   /declare i int local
   /declare castName string local
   /declare spellTime int local
   /declare debuffResult string local
   /declare castSpell spell local
   /declare timerName string local
   /declare minDuration int local 30
   /declare requireMana int local 0
   /declare entry string local
   /declare maxResists int local 5
   /declare currentResistCount int local 0
   /declare cancelConditions string local
   /declare lifeDeficit int local
   /declare lifeRatio int local


   /for i 1 to ${iniDebuffs.Size}
      /varset currentResistCount 0
      /varset cancelConditions TARGETDEAD

      /if (${iniDebuffs[${i},1].Equal[-]}) /goto :loopesc


      /if (${iniDebuffs[${i},${bTargetOverLevel}].NotEqual[-]}) {
         /if (${Spawn[id ${targetID}].Level} < ${iniDebuffs[${i},${bTargetOverLevel}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bTargetUnderLevel}].NotEqual[-]}) {
         /if (${Spawn[id ${targetID}].Level} >= ${iniDebuffs[${i},${bTargetUnderLevel}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bSelfOverMana}].NotEqual[-]}) {
         /if (${Me.PctMana} < ${iniDebuffs[${i},${bSelfOverMana}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bRequireMobsInProximity}].NotEqual[-]}) {
         /if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 45]} < ${iniDebuffs[${i},${bRequireMobsInProximity}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bRequireMaxMobsInProximity}].NotEqual[-]}) {
         /if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 45]} > ${iniDebuffs[${i},${bRequireMaxMobsInProximity}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bRequireTargetClass}].NotEqual[-]}) {
         |/echo ${Spawn[id ${targetID}].Class.ShortName} === ${iniDebuffs[${i},${bRequireTargetClass}]}
         |/echo ${Select[${Spawn[id ${targetID}].Class.ShortName},${iniDebuffs[${i},${bRequireTargetClass}]}]}
         /if (${Select[${Spawn[id ${targetID}].Class.ShortName},${iniDebuffs[${i},${bRequireTargetClass}]}]}==0) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bTargetUnderHP}].NotEqual[-]}) {
         /call fullTarget ${targetID}
         /if (${Target.PctHPs} > ${iniDebuffs[${i},${bTargetUnderHP}]}) /goto :loopnext
      }

      /if (${iniDebuffs[${i},${bTargetOverHP}].NotEqual[-]}) {
         /call fullTarget ${targetID}
         /if (${Target.PctHPs} <= ${iniDebuffs[${i},${bTargetOverHP}]}) /goto :loopnext
         /varset cancelConditions ${cancelConditions},TARGETUNDERHP:${iniDebuffs[${i},${bTargetOverHP}]}

      }

      /if (${iniDebuffs[${i},${bTargetLifeManaRatio}].NotEqual[-]}) {
         /call fullTarget ${targetID}
         /varset lifeDeficit ${Math.Calc[100-${Target.PctHPs}]}
         /varset lifeRatio ${iniDebuffs[${i},${bTargetLifeManaRatio}]}
         /if (${Me.PctMana} < ${Math.Calc[100-(${lifeDeficit}/${lifeRatio})]}) {
            /continue
         }
      }


      /if (${iniDebuffs[${i},${bMaxResists}].NotEqual[-]}) {
         /varset maxResists ${iniDebuffs[${i},${bMaxResists}]}
      }

      /varset castName ${iniDebuffs[${i},1]}

      /if (${isCastReady[${castName}]}==FALSE) /goto :loopnext

      /call e3_getSpellByCastRequestName "${castName}"
      /if (${Macro.Return.Equal[UNKNOWN]}) /goto :loopesc
      /vardata castSpell ${Macro.Return}

      /varset spellTime ${castSpell.Duration.TotalSeconds}

      /if (${iniDebuffs[${i},${bRecastDelay}].NotEqual[-]}) {
         /varset spellTime ${Math.Calc[${spellTime} + ${iniDebuffs[${i},${bRecastDelay}]}]}
      }

      /varset timerName detriTimer_${targetID}_${castSpell.ID}

      | ALREADY TIMER FOR IT? ITS LOCKED OUT...
      /if (${Defined[${timerName}]}==TRUE) {
         /if (${${timerName}}>0) /goto :loopnext
         /deletevar ${timerName}
      }

      |:recastDebuff
      /if (${FindItemCount[${castName}]}>0) {
         /call waitForInt "FindItem[${castName}].TimerReady" 0
      }

      /if (${Spawn[id ${targetID}].Dead}==TRUE) {
         /varset debuffResult TARGET_DEAD
         /goto :loopesc
      }

      /if (${iniDebuffs[${i},${bMemInGem}].NotEqual[-]}) {
         /if (${Me.Gem[${castName}]}==NULL) {
            /call memorizeSpellInGem "${castName}" ${iniDebuffs[${i},${bMemInGem}]}
         }
      }

      :resistRetry

      | CAST THE DEBUFF
      /call e3_Cast ${targetID} "${castName}" "${cancelConditions}"
      /varset debuffResult ${Macro.Return}
      
      /bc ${castName}(${Spawn[id ${targetID}].CleanName}) RES=> ${debuffResult}

      /if (${debuffResult.Equal[LOS]}) /goto :loopesc
      /if (${debuffResult.Equal[CAST_INTERRUPTED]}) /goto :loopesc
      /if (${debuffResult.Equal[CAST_RESIST]}) {
         /varset currentResistCount ${Math.Calc[${currentResistCount}+1]}
         /bc Cast Attempts: ${currentResistCount} of ${maxResists}
         /if (${currentResistCount}<${maxResists}) {
            /while (${isCastReady[${castName}]}==FALSE) {
               /delay 2
            }
            /goto :resistRetry
         }
      }

      | If the cast was successful.
      /if (${debuffResult.Equal[CAST_SUCCESS]}) {
         /if (${spellTime}>0) {
            /declare ${timerName} timer outer ${spellTime}s
         }
      }

      /if (${currentResistCount}==${maxResists}) {
         /bc My spell was resisted too much.  Giving up...
         /em gives up casting ${castName} on ${Spawn[id ${targetID}].CleanName}, too many resists.
         /call lockout ${timerName} 5m         
      }

      | If the spell did not take hold.
      /if (${debuffResult.Equal[CAST_NOTAKEHOLD]} || ${debuffResult.Equal[CAST_IMMUNE]} || ${debuffResult.Equal[CAST_TAKEHOLD]}) {
         /g My spell didn't take hold, or target was immune: ${castName} on ${Spawn[id ${targetID}].CleanName}
         /em gives up casting ${castName} on ${Spawn[id ${targetID}].CleanName}, didn't take hold, or target was immune.
         /call lockout ${timerName} 5m
      }

      | UNKNOWN? skip
      /if (${debuffResult.Equal[UNKNOWN]})  /goto :loopnext

      :loopnext
   /next i
   :loopesc

/RETURN

SUB tauntSuccessListContains(string s)
   /if (${tauntSuccessList.Count}==0) /return FALSE
   /declare i int local

   /for i 0 to ${Math.Calc[${tauntSuccessList.Count}-1]}
      /if (${tauntSuccessList.Item[${i}].Find[${s}]}!=NULL) /return TRUE
   /next i

   /return FALSE
/RETURN



SUB fullTarget(int targetID)
   /if (${Target.ID} != ${targetID}) {
      /tar id ${targetID}
      /delay 10
   }
/RETURN


#EVENT DebuffsOn "<#1#> Debuffs on #2#"
#EVENT DebuffsOn "#1# tells you, 'Debuffs on #2#'"
#EVENT DebuffsOn "#1# tells the group, 'Debuffs on #2#'"
SUB Event_DebuffsOn(line, ChatSender, targetID)

   | DONT MESS W CORPSES
   /if (${Spawn[id ${targetID}].Type.Equal[Corpse]}) /RETURN CORPSE

   | CHECK IF THIS CHARACTER DEBUFFS TO CAST.
   /if (${Defined[iniDebuffs]}==FALSE) /RETURN NO_DEBUFFS_DEFINED

   | CHECK RANGE TO THE DEBUFF TARGET.
   /if (${Spawn[id ${targetID}].Distance} > ${maxAssistDistance}) /RETURN OUT_OF_ASSIST_RANGE

   |/call clearLongTermSpellTimers "${targetID}" "debuffsOnCommand"

   | CHECK IF THE DEBUFF_TARGETS ARRAY IS ALREADY DEFINED
   /if (${debuffTargetIDs.Find[${targetID}_${Zone.ID};]}) /RETURN ALREADY_DEBUFFING

   /call addTargetForDebuffs ${targetID}
/RETURN

SUB addTargetForDebuffs(int targetID) 
   /varset debuffTargetIDs ${debuffTargetIDs}${targetID}_${Zone.ID};
   /echo Added ID.  new list > ${debuffTargetIDs} <
/RETURN


#EVENT debuffsOff "e3 debuffs off"
SUB event_debuffsOff(line, ChatSender, targetID)
   /echo Clearing debuff targets...
   /varset debuffTargetIDs
/RETURN


#event Assist "[#1#(msg)] Assist on #2#"
#event Assist "<#1#> Assist on #2#"
#event Assist "#1# tells you, 'Assist on #2#'"
SUB event_Assist(line, ChatSender, string targetID)
   /if (${NetBots[${ChatSender}].InZone} == FALSE) {
      /bc Cannot assist. Not in the same zoneID.
      /return
   }


   /stick off
   /moveto off
   /face fast id ${targetID}
   /if (${ignoreAssistCalls}==TRUE) /return
   |/if (${tankMode}==TRUE && ${assistTarget}!=0) /return
   /if (${Spawn[${targetID}].Distance} > ${maxAssistDistance}) /return
   /call setAssistOn ${targetID}
/RETURN


#event AssistQueue "[#1#(msg)] Assist queue #2#"
#event AssistQueue "<#1#> Assist queue #2#"
#event AssistQueue "#1# tells you, 'Assist queue #2#'"
SUB event_AssistQueue(line, ChatSender, int targetID)
   /if (${NetBots[${ChatSender}].InZone} == FALSE) {
      /bc Cannot assist. Not in the same zoneID.
      /return
   }

   /if (${ignoreAssistCalls}==TRUE) /return
   /if (${tankMode}==TRUE && ${assistTarget}!=0) /return
   /if (${Spawn[${targetID}].Distance} > ${maxAssistDistance}) /return

   /if (${assistQueue.Find[${targetID},]}!=NULL) {
      /bc Target already queued for assist.
   } else {
      /varset assistQueue ${targetID}|${assistQueue}
   }
/RETURN


#event BackOff "<#1#> assist off"
#event BackOff "[#1#(msg)] assist off"
#event BackOff "#1# tells you, 'assist off'"
SUB Event_BackOff(line, ChatSender)
   /varset autoAssistHoldTimer 30
   /call setAssistOff
/RETURN


#EVENT slayOff "#*#e3 slay off#*#"
SUB event_slayOff(line, ChatSender)
   /bc ${Me.Name} turning slay mode off.
   /varset slayMode FALSE
/RETURN


#EVENT slayOn "#*#e3 slay on#*#"
SUB event_slayOn(line, ChatSender)
   /if (${meleeAssist}==TRUE) {
      /bc ${Me.Name} turning slay mode on.
      /varset slayMode TRUE
      /invoke ${slayProxIDs.Clear}
      /varset slayLockout FALSE
   }
/RETURN

#EVENT burnOn "#*#e3 burn on #1#"
#EVENT burnOn "#*#e3 burns on #1#"
SUB event_burnOn(string line, string burnTime)
   /varset burnTimer ${burnTime}s
   /varset burnsActive TRUE

   /bc Activating burns for ${burnTime} seconds.
/RETURN

#EVENT burnOff "#*#e3 burn off#*#"
#EVENT burnOff "#*#e3 burns off#*#"
SUB event_burnOff(string line, string burnTime)
   /varset burnTimer 0
   /varset burnsActive FALSE

   /bc Burns turned off.
/RETURN

#EVENT allMelee "#*#e3 all melee#*#"
SUB event_allMelee(string line, string burnTime)
   /varset meleeAssist TRUE
   /varset ignoreAssistCalls FALSE
   /bc Will now melee attack on assist.
/RETURN


#event retreatHere "[#1#(msg)] e3 retreat here"
#event retreatHere "<#1#> e3 retreat here"
SUB event_retreatHere(string line, string who)
   /varset meleeRetreatLocXYZ ${Spawn[pc ${who}].X} ${Spawn[pc ${who}].Y} ${Spawn[pc ${who}].Z}
   /varset meleeRetreat TRUE
   /bc Retreach XYZ is now: ${meleeRetreatLocXYZ}
/RETURN

#event retreatOff "[#1#(msg)] e3 retreat off"
#event retreatOff "<#1#> e3 retreat off"
SUB event_retreatOff(string line, string who)
   /varset meleeRetreatLocXYZ -
   /varset meleeRetreat FALSE
   /bc Retreat off.
/RETURN


#event joustMode "[#*#(msg)] e3 joust #1# #2#"
#event joustMode "<#*#> e3 joust #1# #2#"
SUB event_joustMode(string line, string newMode, string dly)
   /if (${newMode.Upper.Equal[ON]}) {
      /varset joustEnabled TRUE
      /varset joustTime ${dly}
      /varset joustDestYXZ ${Me.Y} ${Me.X} ${Me.Z}
      /varset joustWaitAtSafeSpot TRUE
   }

   /if (${newMode.Upper.Equal[OFF]}) /varset joustEnabled FALSE

   /bc Joust on is ${newMode}

   /if (${joustEnabled}==TRUE) {
      /bc JoustTime: ${joustTime}
   }
/RETURN


#event joustTrigger "#*#!joust#*#"
|#event joustTrigger "#*# tells the raid, '!joust'"
SUB event_joustTrigger()
   /varset joustWaitAtSafeSpot FALSE
   /varset joustTimer ${joustTime}
/RETURN

#event charmOn "[#*#(msg)] e3 charm #1#"
SUB event_charmOn(string line, int spawnID)
   /if (${doCharm}==FALSE) /bc I am not configured to accept charm commands. [Assist] => AcceptCharmCommands=NO
   /varset currentCharmSpawnID ${spawnID}
   /bc I will attempt to keep spawnID ${spawnID} ( ${Spawn[id ${currentCharmSpawnID}]} ) charmed.
/RETURN

#event charmOff "[#*#(msg)] e3 charm off"
#event charmOff "<#*#> e3 charm off"
SUB event_charmOff(string line)
   /if (${doCharm}==FALSE) /return
   /if (${currentCharmSpawnID}==0) /return

   /bc I will no longer try to charm spawnID ${currentCharmSpawnID}.

   /varset currentCharmSpawnID 0
/RETURN


#EVENT mezBreak "#*# #1# spell has worn off#*#"
SUB EVENT_mezBreak(string line, string spellName)
   /if (${spellName.Upper.Equal[${charmSpellName.Upper}]}) {
      /g Charm break!
      /if (${charmBreakCommand.NotEqual[NONE]}) {
         /call e3command "${charmBreakCommand}"
      }
   }
/RETURN


#EVENT charmBreak "#1# has been xawakened by#*#"
SUB EVENT_charmBreak(string line, string mobName)
   /declare i int local
   /declare mob spawn local

   /declare mobCount int local ${SpawnCount[${mobName}]}

   | FIND THE CORRECT MOB. INITIATE A RE-MEZ BY REMOVING FROM MEZLIST AND LET MEZ CHECK FIND IT AGAIN
   /for i 1 to ${mobCount}
      /vardata mob NearestSpawn[${i},${mobName}]
      /if (${autoMezList.Index[${mob.ID}]}!=-1) {
         /tar id ${mob.ID}
         /delay 10
         /if (${Target.Mezzed.ID}==NULL) {
            /invoke ${autoMezList.Erase[${autoMezList.Index[${mob.ID}]}]}
            /varset mez_${mob.ID} 0
            /return
         }
      }
   /next i
/RETURN

#event toggleAutomez "[#*#(msg)] e3 automez #1#"
#event toggleAutomez "<#*#> e3 automez #1#"
SUB EVENT_toggleAutomez(string line, string newMode)
   /if (${newMode.Upper.Equal[ON]}) {
      /varset autoMez TRUE
      /echo Automez is now ON
   } else /if (${newMode.Upper.Equal[OFF]}) {
      /varset autoMez FALSE
      /echo Automez is now OFF
   }

/RETURN 

#event tauntSuccess "You capture #*# attention!"
#event tauntSuccess "You capture #*# with your unparalleled reproach!"
SUB EVENT_tauntSuccess(string line, string newMode)
   /if (${Target.ID}==NULL) /return
   
   /invoke ${tauntSuccessList.Append[${Target.ID}_${MacroQuest.Running}]}
/RETURN

#EVENT campCommand "[#1#(msg)] e3 camp #2#"
#EVENT campCommand "<#1#> e3 camp #2#"
SUB EVENT_campCommand(string line, string source, string newMode)
   /if (${Spawn[${source}].ID}==NULL) {
      /bc Can't find "${source}" to use as the camp location.
      /return
   }
   
   /if (${newMode.Upper.Equal[OFF]}) {
      /varset campEnabled FALSE
      /bc Camp mode disabled.
   }

   /if (${newMode.Upper.Equal[HERE]}) {
      /varset campX ${Spawn[${source}].X}
      /varset campY ${Spawn[${source}].Y}
      /varset campZ ${Spawn[${source}].Z}
      /varset campEnabled TRUE
      /bc Now camping at ${source} location.
   }

   /if (${newMode.Upper.Equal[THERE]}) {
      /varset campX ${Me.X}
      /varset campY ${Me.Y}
      /varset campZ ${Me.Z}
      /varset campEnabled TRUE
      /bc Now camping at my location.
   }

/RETURN


#event toggleAutofire "[#*#(msg)] e3 autofire #1#"
#event toggleAutofire "<#*#> e3 autofire #1#"
SUB EVENT_campCommand(string line, string newMode)
   /if (${newMode.Upper.Equal[ON]}) {
      /varset auotFireAssist TRUE
      /varset meleeAssist FALSE
      /varset tankMode FALSE
      /bc Autofire is ON.
   } else /if (${newMode.Upper.Equal[OFF]}) {
      /varset auotFireAssist FALSE
      /if (${autoFireTankMode}==TRUE) /varset tankMode TRUE
      /if (${autoFireMeleeMode}==TRUE) /varset meleeAssist TRUE
      /bc Autofire is OFF.
   } else {
      /bc Unrecognized option ${newMode}.  Use ON or OFF.
      /return
   }
/RETURN