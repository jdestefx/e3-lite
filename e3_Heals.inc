SUB heals_CharacterSettings(bool backFillOnly)
	/call WriteToIni "${iniFileCharacter},Misc,XTargetHealsEnabled" NO ${backFillOnly}
/RETURN

SUB heals_Setup
	/declare healsEnabled bool outer TRUE
	/declare healSpells[0] string outer
	/declare otherHealerNetBots string outer
	/declare healIndex int outer 1
	/declare enableHealIndex bool outer TRUE
	/declare checkHealerIndexTimer timer outer 0
	/declare healerCheckTimer timer outer 0
	/declare xTargetHealsEnabled bool outer FALSE

	/declare curseRemovals[0] string outer
	/declare poisonRemovals[0] string outer
	/declare doCurseRemoval bool outer TRUE
	/declare doPoisonRemoval bool outer TRUE

	/call getIniToVar "${iniFileCharacter},Heal Team,OtherHealerNetBots" otherHealerNetBots NONE
	/call getIniToVar "${iniFileCharacter},Misc,XTargetHealsEnabled" xTargetHealsEnabled NO

	/call processHealTargets
/RETURN

SUB processHealTargets
	/if (${Defined[iniHeals]}==FALSE) /return

	/declare i int local 1
	/declare e string local 
	/declare j int local 1
	/declare sectionText string local
	/declare sectionValue string local
	/declare checkDefinitionName string local

	/for i 1 to 64
		/if (${iniHeals[${i},1].Equal[-]}) /goto :loopesc

		/if (${iniHeals[${i},${bTargets}].NotEqual[-]}) {
			/call processTargets "${iniHeals[${i},${bTargets}]}"
			/varset iniHeals[${i},${bTargets}] ${Macro.Return}
		} else {
			/varset iniHeals[${i},${bTargets}] ${allNetbotNames}
		}

		/varset iniHeals[${i},${bTargets}] ${iniHeals[${i},${bTargets}].Replace[|,,]}			

		|/if (${iniHeals[${i},${bExcludeTargets}].NotEqual[-]}) {
		|	/call processTargets "${iniHeals[${i},${bExcludeTargets}]}"
		|	/varset iniHeals[${i},${bExcludeTargets}] ${Macro.Return}
		|}
		|
		|/varset iniHeals[${i},${bExcludeTargets}] ${Macro.Return.Replace[|,,]}

	/next i
	:loopesc
/RETURN

SUB heal_Background_Events
	/if (${checkHealerIndexTimer}==0) /call determineHealerIndex
	
	/if (${healsEnabled}==TRUE && ${Defined[iniHeals]}) {
		/call checkHeals
	}
/RETURN

SUB check_CurseRemoval 
	/declare i int local 0
	/declare netbotName string local

	/for i 1 to ${netbotCount}
		/varset netbotName ${NetBots.Client[${i}]}

		/if (${NetBots[${netbotName}].Cursed}) {

		}
	/next i
/RETURN


SUB determineHealerIndex
	/if (${otherHealerNetBots.Equal[NONE]}) /return

	/if (${checkHealerIndexTimer}>0) /return
	/varset checkHealerIndexTimer 2s

	/declare i int local
	/declare j int local
	/declare entryCount int local ${Math.Calc[${otherHealerNetBots.Count[,]}+1]}
	/declare entryName string local
	/declare myManaAmount int local
	/declare entryManaAmount int local

	/declare highMana int local 101

	| RESET TO 1 FOR THIS SCAN
	/varset healIndex ${Math.Calc[${entryCount}+1]} 

	|/if (${Me.Stunned}==TRUE || ${Me.SpellInCooldown}==TRUE) {
	|   /varset healIndex 100
	|   /return
	|}

	/for i 1 to ${entryCount}
		/varset entryName ${otherHealerNetBots.Arg[${i},,]}
		/varset entryName ${entryName.Left[1]}${entryName.Right[-1].Lower}

		/varset myManaAmount ${Me.PctMana}
		/varset entryManaAmount ${NetBots[${entryName}].PctMana}

		| ARE THEY CASTING SOMETHING? BUMP ME UP
		|/if (${Bool[${NetBots[${entryName}].Casting}]}==TRUE) {
		|   /varset healIndex ${Math.Calc[${healIndex}-1]}
		|   /continue
		|}

		/if (${NetBots[${entryName}].InZone}==FALSE) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
			/continue
		}

		| IS ANOTHER NETBOT STUNNED OR DEAD? INCREASE MY PRIORITY
		/if (${Select[${NetBots[${entryName}].State},DEAD,STUN,HOVER]}>0) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
			/goto :loopend
		}

		| DOES MY MANA MATCH ANOTHER EXACTLY? ONE OF US NEEDS TO DEFER - USE ID AS A SEQUENTIAL DIFFERENTIATOR
		/if (${entryManaAmount}==${myManaAmount}) {
			| IF MY ID IS HIGHER THAN THEIR ID, INCREASE MY PRI
			/if (${Me.ID}>${Spawn[PC ${entryName}].ID}) {
				/varset healIndex ${Math.Calc[${healIndex}-1]}
			}
		} else /if (${myManaAmount} > ${entryManaAmount}) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
		}
		:loopend
	/next i

/RETURN

SUB checkHeals
	/if (${canDoHeals}==FALSE) /RETURN
	/if (${Me.Moving}==TRUE) /RETURN
	/if (${iniHeals.Size}==0) /RETURN
	/if (${Stick.Active}==TRUE) /RETURN
	/if (${Me.Silenced.ID}!=NULL) /RETURN
	/if (${Me.Invis}==TRUE) /RETURN

	/if (${healerCheckTimer}>0) /RETURN
	/varset healerCheckTimer 1s

	/declare i int local
	/declare cancelConditions string local
	/declare healEntry string local
	/declare healTargetName string local
	/declare healSpawn spawn local
	/declare healSpellName string local
	/declare healSpell spell local
	/declare healPct int local 90
	/declare useHealIndex bool local FALSE
	/declare checkSpellID int local
	/declare healLockoutTimerName string local
	/declare triggerIdleCasting bool local TRUE

	/declare botBuffIDs string local 

	/for i 1 to ${iniHeals.Size}
		/if (${Stick.Active}==TRUE) /goto :loopend
		/varset triggerIdleCasting TRUE
		/varset botBuffIDs
		/varset healEntry ${iniHeals[${i},1]}
		/if (${healEntry.Equal[-]}) /goto :loopend

		/varset healSpellName ${iniHeals[${i},1]}
		/varset healLockoutTimerName heal_${healSpawn.ID}_${healSpellName}

		/if (${iniHeals[${i},${bRequireCastIdle}].NotEqual[-]}) {
			/if (${idleCastingTimer}>0) /continue
		}

		/if (${iniHeals[${i},${bCastableOnlyResting}].NotEqual[-]}) {
			/if (${Me.CombatState.Equal[COMBAT]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfOverHP}].NotEqual[-]}) {
			/if (${Me.PctHPs} < ${iniHeals[${i},${bSelfOverHP}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfUnderHP}].NotEqual[-]}) {
			/if (${Me.PctHPs} >= ${iniHeals[${i},${bSelfUnderHP}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfOverMana}].NotEqual[-]}) {
			/if (${Me.PctMana} < ${iniHeals[${i},${bSelfOverMana}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfUnderMana}].NotEqual[-]}) {
			/if (${Me.PctMana} >= ${iniHeals[${i},${bSelfUnderMana}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bRequireMaxMobsInProximity}].NotEqual[-]}) {
			/if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 75]} > ${iniHeals[${i},${bRequireMaxMobsInProximity}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bRequireHurtPartyMembers}].NotEqual[-]}) {
			/if (${checkHurtPartyMembers[${iniHeals[${i},${bRequireHurtPartyMembers}]},45]}==FALSE) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bUseHealIndex}].NotEqual[-]}) {
			/varset useHealIndex TRUE
		} else {
			/varset useHealIndex FALSE
		}

		/if (${iniHeals[${i},${bHealPct}].NotEqual[-]}) /varset healPct ${iniHeals[${i},${bHealPct}]}
		/call getNetBotReadyForHeal ${healPct} ${i}

		/if (${Macro.Return.Equal[NONE]}) /goto :loopnexti

		/if (${useHealIndex}==TRUE && ${enableHealIndex}==TRUE && ${otherHealerNetBots.NotEqual[NONE]}) {
			/varset healTargetName ${Macro.Return}
			/if (${healTargetName.Count[;]} < ${healIndex}) /goto :loopnexti
			/varset healTargetName ${healTargetName.Arg[${healIndex},;]}
		} else {
			/varset healTargetName ${Macro.Return}
			/varset healTargetName ${healTargetName.Arg[1,;]}
		}

		/if (${iniHeals[${i},${bDoCommand}].NotEqual[-]}) /docommand ${iniHeals[${i},${bDoCommand}]}

		/vardata healSpawn Spawn[PC ${healTargetName}]
		/call e3_getSpellByCastRequestName "${healSpellName}"
		/if (${Macro.Return.Equal[UNKNOWN]} || ${Macro.Return.Equal[NOTREADY]} || ${Macro.Return.Equal[COOLDOWN]}) /goto :loopnexti
		/vardata healSpell ${Macro.Return}

		/if (${iniHeals[${i},${bRecastDelay}].NotEqual[-]}) {
			/if (${isLockoutExpired[heal_${healSpawn.ID}_${healSpell.ID}]}==FALSE) /goto :loopnexti
		}

		/if (${healSpawn.Distance3D} > ${healSpell.MyRange} && ${healSpawn.Distance3D} > ${healSpell.AERange}) /goto :loopnexti
		|/if (${healSpawn.Distance3D} > ${healSpell.MyRange}) /goto :loopnexti

		/if (${iniHeals[${i},${bNotIfShortBuff}].NotEqual[-]}) {
			/varset checkSpellID ${Spell[${iniHeals[${i},${bNotIfShortBuff}]}].ID}
			/if (${Select[${checkSpellID},${NetBots[${healSpawn.Name}].ShortBuff}]}>0) {
				/goto :loopnexti
			}
		}

		| CHECK SHORTBUFFS
		/if (${iniHeals[${i},${bCheckShortBuffs}].NotEqual[-]}) {
			/varset botBuffIDs ${NetBots[${healSpawn.Name}].ShortBuff}
			/if (${botBuffIDs.Find[${healSpell.ID}]}!=0) /goto :loopnexti
		}
		/if (${iniHeals[${i},${bCheckLongBuffs}].NotEqual[-]}) {
			/varset botBuffIDs ${NetBots[${healSpawn.Name}].ShortBuff}
			/if (${botBuffIDs.Find[${healSpell.ID}]}!=0) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bNoCancelChecks}].Equal[-]}) {
			/varset cancelConditions TARGETLIFEPCT:${healPct}|TARGETHASBUFF:${healSpell.Name}
		} else {
			/varset cancelConditions TARGETHASBUFF:${healSpell.Name}
		}

		/if (${iniHeals[${i},${bMemInGem}].NotEqual[-]}) {
			/if (${Me.Gem[${healEntry}]}==NULL) {
				/call memorizeSpellInGem "${healEntry}" ${iniHeals[${i},${bMemInGem}]}
			}
		}

		/if (${iniHeals[${i},${bRequireIdleCasting}].NotEqual[-]}) {
			/if (${idleCastingTimer}>0) /return
			/varset triggerIdleCasting FALSE
		}

		/call e3_Cast ${healSpawn.ID} "${healEntry}" "${cancelConditions}" ${triggerIdleCasting}

		/if (${Macro.Return.Equal[CAST_SUCCESS]}) {
			/if (${iniHeals[${i},${bRecastDelay}].NotEqual[-]}) {
				/varset healLockoutTimerName heal_${healSpawn.ID}_${healSpell.ID}
				/call lockout ${healLockoutTimerName} ${iniHeals[${i},${bRecastDelay}]}
			}

			/if (${iniHeals[${i},${bNoSitTimer}].NotEqual[-]}) /call pauseMedBreak ${iniHeals[${i},${bNoSitTimer}]}
			
			/return TRUE
		} else {
			/echo CastResult: ${Macro.Return}
			/return
		}

		:loopnexti
	/next i
	:loopend

	/return FALSE

/RETURN

SUB checkHurtPartyMembers(string params, int distanceLimit)
	/declare requireHurtChars int local ${params.Token[1,@]}
	/declare requireHealthPct int local ${params.Token[2,@]}
	/declare hurtChars int local 0

	/if (${Me.Grouped}==FALSE) /return FALSE

	/declare i int local 0
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
		/if (${Group.Member[${i}].Dead}==TRUE) /continue
		/if (${distanceLimit} > 0 && ${Group.Member[${i}].Distance} > ${distanceLimit}) /continue
		/if (${Group.Member[${i}].PctHPs} > ${requireHealthPct}) /continue

		/varset hurtChars ${Math.Calc[${hurtChars}+1]}
	/next i

	/if (${hurtChars}>= ${requireHurtChars}) /return TRUE

	/return FALSE

/RETURN

SUB getNetBotReadyForHeal(int healPct, int iniHealIndex)
	/declare i int local 0
	/declare j int local 0
	/declare netbotName string local
	/declare netbotClass string local
	/declare netbotCount int local ${NetBots.Counts}
	/declare lowestPct int local 100
	/declare healNames list local
	/declare li listiterator local
	/declare b string local
	/declare healSpell spell local
	/declare netbotShortBuffIDs string local

	/if (${xTargetHealsEnabled}==TRUE && (${iniHeals[${iniHealIndex},${bXTargetHeal}].NotEqual[-]} || ${iniHeals[${iniHealIndex},${bXTargetOnlyHeal}].NotEqual[-]}) )  {
		/for i 1 to 12
			/if (${Me.XTarget[${i}].ID}==NULL) /continue
			/if (${Me.XTarget[${i}].ID}==0) /continue
			/if (${Me.XTarget[${i}].Type.NotEqual[PC]}) /continue
			/if (${Me.XTarget[${i}].PctHPs} <= ${healPct}) {
				/if (${healNames.Index[${Me.XTarget[${i}].CleanName}]}!=-1) /continue
				/if (${isLockoutExpired[heal_${Me.XTarget[${i}].ID}_${iniHeals[${iniHealIndex},1]}]}==FALSE) /continue

				/if (${Me.XTarget[${i}].PctHPs}<${lowestPct}) {
					/varset lowestPct ${Me.XTarget[${i}].PctHPs}
					/invoke ${healNames.Insert[0,${Me.XTarget[${i}].CleanName}]}
				} else {
					/invoke ${healNames.Append[${Me.XTarget[${i}].CleanName}]}
				}
			}
		/next i
	}

	/if (${iniHeals[${iniHealIndex},${bXTargetOnlyHeal}].NotEqual[-]}) /goto :doneWithNames

	/if (${Me.Grouped}==TRUE && ${iniHeals[${iniHealIndex},${bIncludeGroup}].NotEqual[-]}) {
		/for i 1 to 5
			/if (${Group.Member[${i}].ID}==NULL) /continue
			/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
			/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
			/if (${Group.Member[${i}].PctHPs} <= ${healPct}) {
				/if (${healNames.Index[${Group.Member[${i}]}!=-1) /continue				
				/if (${isLockoutExpired[heal_${Group.Member[${i}].ID}_${iniHeals[${iniHealIndex},1]}]}==FALSE) /continue
				/if (${Group.Member[${i}].PctHPs}<${lowestPct}) {
					/varset lowestPct ${Group.Member[${i}].PctHPs}
					/invoke ${healNames.Insert[0,${Group.Member[${i}]}]}
				} else {
					/invoke ${healNames.Append[${Group.Member[${i}]}]}
				}
			}
		/next i
	}


	/for i 1 to ${netbotCount}
		/varset netbotName ${NetBots.Client[${i}]}
		/if (${netbotName.Equal[NULL]}) /break

		/varset netbotClass ${NetBots[${netbotName}].Class.ShortName}

		/if (${NetBots[${netbotName}].InZone} == FALSE) /continue

		/if (${Spawn[${netbotName}].Dead}==TRUE) /continue

		/if (${NetBots[${netbotName}].State.Equal[DEAD]}==TRUE) /continue

		/if (${NetBots[${netbotName}].Invulnerable}==1) /continue

		/if (${iniHeals[${iniHealIndex},${bCheckShortBuffs}].NotEqual[-]}) {
			/vardata healSpell ${e3_getSpellByCastRequestName[${iniHeals[${iniHealIndex},1]}]}
			/if (${NetBots[${netbotName}].ShortBuff.Find[${healSpell.ID}]}!=0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bTargets}].NotEqual[-]}) {
			/if (${Select[${netbotName},${iniHeals[${iniHealIndex},${bTargets}]}]}==0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireGroup}].NotEqual[-]}) {
			/if (${Group.Member[${netbotName}].ID}==NULL) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bTargetUnderLevel}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Level} >= ${iniHeals[${iniHealIndex},${bTargetUnderLevel}]}) /continue
		}
		/if (${iniHeals[${iniHealIndex},${bTargetOverLevel}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Level} <= ${iniHeals[${iniHealIndex},${bTargetOverLevel}]}) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireDiseased}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Diseased} == 0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequirePoisoned}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Poisoned} == 0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireCursed}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Cursed} == 0) /continue
		}

		| bRequireHurtPartyMembers implies group memeber, so verify
		/if (${iniHeals[${iniHealIndex},${bRequireHurtPartyMembers}].NotEqual[-]}) {
			/if (${Group.Member[${netbotName}].ID}==NULL) /continue
		}

		/if (${NetBots[${netbotName}].PctHPs} <= ${healPct}) {
			/if (${healNames.Index[${netbotName}]}!=-1) /continue
			/if (${NetBots[${netbotName}].PctHPs} < ${lowestPct}) {
				/varset lowestPct ${NetBots[${netbotName}].PctHPs}
				/invoke ${healNames.Insert[0,${netbotName}]}
			} else {
				/invoke ${healNames.Append[${netbotName}]}
			}
		}

	/next i

	:doneWithNames

	/if (${healNames.Count}==0) /return NONE

	/vardata li healNames.First
	/while (${li.IsEnd}==FALSE) {
		/varset b ${b}${li.Value}
		/invoke ${li.Advance}
		/if (${li.IsEnd}==TRUE) /break
		/varset b ${b};
	}

	/return ${b};
/RETURN

#EVENT xthModeChange "#*#e3 xth #1#"
SUB event_xthModeChange(string line, string xthMode)
	/if (${xthMode.Upper.Equal[ON]}) {
		/varset xTargetHealsEnabled TRUE
	} else /if (${xthMode.Upper.Equal[ON]}) {
		/varset xTargetHealsEnabled FALSE
	} else {
		/bc Unknown XTH mode. Use on or off.
		/return
	}

	/bc XTargetHeals turned ${xthMode}.
/RETURN