
SUB IniToVar(Ini_Key, VarToMake, VarType, VarScope)

	| Check for the specified ini key.
	/if (!${Ini[${Ini_Key}].Length}) {
		|/if (${Debug}) /echo [${Ini_Key}] not found.
	} else {
		
		/declare iniValue string local ${Ini[${Ini_Key}]}

		/if (${Defined[${VarToMake}]}==TRUE) /deletevar ${VarToMake}

		| Validate bool.
		/if (${VarType.Equal[bool]}) {
			/declare ${VarToMake} bool ${VarScope} ${If[${Select[${iniValue},TRUE,ON,YES,1]},TRUE,FALSE]}

		| Validate int.
		} else /if (${VarType.Equal[Int]}) {
			/declare ${VarToMake} int ${VarScope} ${iniValue}
		| Validate float.
		} else /if (${VarType.Equal[Float]}) {
			/declare ${VarToMake} float ${VarScope} ${iniValue}
		| Create string or timer.
		} else {
			/declare ${VarToMake} ${VarType} ${VarScope} ${iniValue}
		}
	}
/RETURN ${${VarToMake}}

SUB e3command(string cmd)
	/varset cmd ${cmd.Replace[`,\\"].Replace[\,]}

	/if (${cmd.Find[!!]}!=NULL) {
		/docommand ${cmd.Replace[!!,$]}
	} else {
		/docommand ${cmd}
	}
/RETURN

SUB getIniToVar(string iniKey, string varName, string defaultResponse)
	/declare iniValue string local ${Ini[${iniKey}]}

	/if (${iniValue.Find[!!]}!=NULL) {
		/varset ${varName} ${iniValue}
		/return
	} else {
		/varset iniValue ${iniValue.Upper}
	}


	/if (${Bool[${iniValue}]}==TRUE) {
		/if (${iniValue.Upper.Equal[ON]} || ${iniValue.Upper.Equal[YES]} || ${iniValue.Upper.Equal[TRUE]}) {
			/varset ${varName} TRUE
		} else /if (${iniValue.Upper.Equal[OFF]} || ${iniValue.Upper.Equal[FALSE]} || ${iniValue.Upper.Equal[NO]}) {
			/varset ${varName} FALSE
		} else {
			/varset ${varName} ${iniValue}
		}
	} else {
		/varset ${varName} ${defaultResponse}
	}
/RETURN

SUB IniToArray(Ini_Key, ArrayName, ArrayScope)

	| Check for the specified ini key.

	/if (!${Ini[${Ini_Key}].Length}) {
		/echo [${Ini_Key}] not found.
		
		/if (${Defined[${ArrayName}]}) {
			/echo deletevarx ${ArrayName}
			/deletevar ${ArrayName}
		}

		/if (!${Defined[ArrayScope]}) {
			/declare ArrayScope string local global
		}

		/declare ${ArrayName}[0] string ${ArrayScope}
		/return ${ArrayName}

	} else {
		| Count ini entries
		/declare i int local 1
		/declare count int local

		:CountingLoop
		/if (${Ini[${Ini_Key}${i}].Length}) {
			/varset count ${i}
			/varcalc i ${i} + 1
			/goto :CountingLoop
		}
		

		| Declare the array and copy ini entries
		/if (${Defined[${ArrayName}]}) {
			/echo deletevar ${ArrayName}
			/deletevar ${ArrayName}
		}

		/if (!${Defined[ArrayScope]}) {
			/declare ArrayScope string local global
		}

		/declare ${ArrayName}[${count}] string ${ArrayScope}
		/echo DECLARE ${ArrayName}[${count}] string ${ArrayScope}
		
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}[${i}] ${Ini[${Ini_Key}${i}]}
			/echo ${ArrayName}[${i}] ${Ini[${Ini_Key}${i}]}
		/next i
		
	}
/RETURN ${ArrayName}

SUB WriteToIni(string iniFile, string writeContents, bool backFillOnly)
	/if (${backFillOnly}==TRUE) {
		/if (${Ini[${iniFile}].Length}==NULL) {
			/bc Populating missing value: "${iniFile.Arg[2,,]}" "${iniFile.Arg[3,,]}" "${writeContents}"
			/ini "${iniFile.Arg[1,,]}" "${iniFile.Arg[2,,]}" "${iniFile.Arg[3,,]}" "${writeContents}"
		}
		/return
	}
	/ini "${iniFile.Arg[1,,]}" "${iniFile.Arg[2,,]}" "${iniFile.Arg[3,,]}" "${writeContents}"
/RETURN

SUB WriteToIniAppend(iniParams, appendVal, joiner)
	/if (!${Defined[joiner]}) {
		/declare joiner string local ,
	}
	|/if (${joiner.Length}==0) /varset joiner ,
	
	/declare file string local ${iniParams.Arg[1,,]}
	/declare section string local ${iniParams.Arg[2,,]}
	/declare name string local ${iniParams.Arg[3,,]}
	/declare writeVal string local

	/declare v string local -
	/varset v ${Ini[${file},${section},${name},-]}

	/if (${v.Equal[-]}) {
		/varset writeVal ${appendVal}
	} else {
		/varset writeVal ${v}${joiner}${appendVal}
	}

	/ini "${file}" "${section}" "${name}" "${writeVal}"
/RETURN

SUB ArrayFindElementByText(array, elementText)
	/declare i int local

	/for i 1 to ${array.Size}
		/if (${array[${i}].Equal[${elementText}]}) /return ${i}
	/next i

	/return -1
/RETURN

SUB BuildArray(ArrayName, Data, varScope)

	/if (${varScope.Length}==0) /varset varScope outer
	
	| If the array is not defined, declare it.
	/if (!${Defined[${ArrayName}]}) {
		/declare ${ArrayName}[1] string ${varScope}
		/varset ${ArrayName}[1] ${Data}
		
	| Else, if the array is already defined.
	} else {
	
		| Create a copy of the existing array.
		/declare ${ArrayName}Copy[${${ArrayName}.Size}] string local
		/declare i int local
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}Copy[${i}] ${${ArrayName}[${i}]}
		/next i
		
		| Delete the existing array.
		/deletevar ${ArrayName}
		
		| Build a resized array.
		/declare ${ArrayName}[${Math.Calc[${${ArrayName}Copy.Size}+1].Int}] string ${varScope}
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}[${i}] ${${ArrayName}Copy[${i}]}
		/next i
	
		| Insert new data into the array
		/varset ${ArrayName}[${${ArrayName}.Size}] ${Data}
	}
/RETURN ${ArrayName}

SUB RemoveArrayElement(ArrayName, Data)
	/declare remainingElements string local
	/declare newArraySize int local

	/declare i int local

	| Count array elements that do not match 'Data'.
	/for i 1 to ${${ArrayName}.Size}	

		/if (${${ArrayName}[${i}].NotEqual[${Data}]}) {
			/varset remainingElements ${remainingElements}${${ArrayName}[${i}]},
			/varcalc newArraySize ${newArraySize} + 1
		}

	/next i
	
	| If there are  elements remaining in the array, create a copy of the new array
	/if (${newArraySize}) {
	
		/declare newArray[${newArraySize}] string local
		
		/for i 1 to ${newArraySize}
		
			/varset newArray[${i}] ${remainingElements.Arg[${i},,]}
		
		/next i
	}
	
	| Delete the existing array.
	/deletevar ${ArrayName}
	
	| If there is a new array to make
	/if (${Defined[newArray]}) {

		/declare ${ArrayName}[${newArray.Size}] string outer
	
		/for i 1 to ${newArray.Size}
		
			/varset ${ArrayName}[${i}] ${newArray[${i}]}
		
		/next i
	}
/RETURN

SUB getEntryParam(string entry, string param, string sep1, string sep2)
	|/if (!${Defined[sep1]}) {
	|   /declare sep1 string local |
	|}
	|/if (!${Defined[sep2]}) {
	|   /declare sep2 string local :
	|}

	/if (${sep1.Length}==0) /varset sep1 |
	/if (${sep2.Length}==0) /varset sep2 :


	/declare e string local
	/declare i int local 
	/declare loopCount int local ${Math.Calc[${entry.Count[${sep1}]}+1]}
	/for i ${loopCount} downto 1
	/varset e ${entry.Arg[${i},${sep1}]}

	/if (${e.Upper.Find[${param}]}==0) {
		/goto :loopend
	} else {
		/if (${e.Find[${sep2}]}==0) /RETURN ${e}
		/RETURN ${e.Arg[2,${sep2}]}
	}

		:loopend
	/next i

	/RETURN MISSING
/RETURN

SUB getIniSectionToArray(string iniKey, string arrayName)

	/declare iniSectionKeys string local
	/declare keyCount int local
	/declare i int local

	/deletevar ${arrayName}

	/varset iniSectionKeys ${Ini[${iniKey}]}

	| EMPTY SECTION? MAKE AN EMPTY ARRAY AND BAIL
	/if (${iniSectionKeys.Equal[NULL]}) {
		/declare ${arrayName}[0] string outer
		/return
	}

	/varset keyCount ${Math.Calc[${iniSectionKeys.Count[|]}-1]}

	| re-create the array with the appropriate size
	/declare ${arrayName}[${keyCount}] string outer

	| fetch and store the ini sections into the array
	/for i 1 to ${keyCount}
		/varset ${arrayName}[${i}] ${iniSectionKeys.Arg[${i},|].Arg[1,#]}|${Ini[${iniKey},${iniSectionKeys.Arg[${i},|]}]}
	/next i
/RETURN


| EXP Spam
#event expSpammerResponder "#*#You gained raid experience#*#"
#event expSpammerResponder "#*#You gain party experience#*#"
#event expSpammerResponder "#*#You gain experience#*#"
SUB event_expSpammerResponder
	/if (${expSpam}==TRUE) {
		/delay 2
		/declare newAAExp int local ${Me.AAExp}
		/declare newExp int local ${Me.Exp}

		/if (${expAmt} < ${newExp}) /bc REG ► +${Math.Calc[(${newExp}-${expAmt}) /1000 ].Precision[3]} [${Me.PctExp.Int}%] (${Me.Level})
		/if (${aaExpAmt} < ${newAAExp}) /bc AA ► +${Math.Calc[(${newAAExp}-${aaExpAmt}) /1000 ].Precision[3]} [${Me.PctAAExp.Int}%] (${Me.AAPoints} Banked)

		/varset expAmt ${newExp}
		/varset aaExpAmt ${newAAExp}
	}
/return

SUB argueString(WhatToFind, givenData)
/if (${Debug}) /echo |- argueString ==>

	/declare i int local 1
	/declare foundData string local NULL
	
	:nextArg
	/if (${givenData.Find[/]}) {
		
		/if (${givenData.Arg[${i},/].Find[${WhatToFind}]}) {
		
			/varset foundData ${givenData.Arg[${i},/].Arg[2,|]}
			/if (${Debug}) /echo |- argueString -| Found [${WhatToFind}] - [${foundData}]
		
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i},/].Length}) /goto :nextArg
		}
	} else {
		/if (${givenData.Arg[${i}, ].Find[${WhatToFind}]}) {
		
			/varset foundData ${givenData.Arg[${i}, ].Arg[2,|]}
			/echo ${givenData.Arg[${i}, ].Arg[2,|]}
			/if (${Debug}) /echo |- argueString -| Found [${WhatToFind}] - [${foundData}]
		
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i}, ].Length}) /goto :nextArg
		}
	}

/if (${Debug}) /echo <== argueString -|
/RETURN ${foundData}

SUB check_selectedBots(selectedBots, ChatSender)

	/declare botSelected bool local FALSE
	/if (${selectedBots.Find[ ]}) /varset selectedBots ${selectedBots.Replace[ ,,]}

	/if (${Select[priests,${selectedBots}]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${Select[healers,${selectedBots}]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${Select[ranged,${selectedBots}]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${Select[casters,${selectedBots}]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${Select[melee,${selectedBots}]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${Select[me,${selectedBots}]} && ${ChatSender.Equal[${Me.CleanName}]} || ${Select[${Me.Class},${selectedBots}]} || ${Select[${Me.Class.ShortName},${selectedBots}]} || ${Select[${Me.CleanName},${selectedBots}]} || ${Select[all,${selectedBots}]} || ${Select[group,${selectedBots}]} && ${Me.CleanName.Equal[${ChatSender}]} || ${Select[group,${selectedBots}]} && ${Group.Member[${ChatSender}].Index} || ${Select[zone,${selectedBots}]} && ${SpawnCount[pc ${ChatSender}]}) /varset botSelected TRUE
/RETURN ${botSelected}


SUB quit (string message) 
	/if (${message.Length}>0) /echo ${message}
	/end
/RETURN

SUB ensureNoItemDisplayWindows
	:again
	/if (${Window[ItemDisplayWindow]}==TRUE) {
		/keypress ESC
		/delay 1
		/goto :again
	}
/RETURN

SUB ensureNoLootWindow
	:again
	/if (${Window[LootWnd]}==TRUE) {
		/keypress ESC
		/delay 3
		/goto :again
	}
/RETURN

SUB waitForBool(string eval, bool result, string docmd)
	:loopstart
		/doevents
		/delay 2
		/if (${stopWait}) {
			/varset stopWait FALSE
			/return
		}
		/if (${docmd.Length}>0) /docommand ${docmd}
		/if (${${eval}}!=${result}) /goto :loopstart
/RETURN

SUB waitForString(string eval, string result, string docmd)
	/declare a string local
	:loopstart
		/delay 2
		/if (${docmd.Length}>0) /docommand ${docmd}
		/varset a ${${eval}}
		/if (${a.Upper.NotEqual[${result.Upper}]}) /goto :loopstart
/RETURN

SUB waitForInt(string eval, int result)
	:loopstart
		/doevents
		/delay 1
		/if (${stopWait}) {
			/varset stopWait FALSE
			/return
		}
		/if (${${eval}}!=${result}) /goto :loopstart
/RETURN

SUB waitForIntLTE(string eval, int result)
	:loopstart
		/doevents
		/delay 1
		/if (${stopWait}) {
			/varset stopWait FALSE
			/return
		}

		/if (${${eval}}>${result}) /goto :loopstart
/RETURN

SUB processArray(string sourceArray, string destArray)
	/declare i int local 1
	/declare e string local  | each line in the array. ie:  Courage=SelfOverEnd:50|SelfOverMana:50

	/declare j int local 1
	/declare sectionText string local 
	/declare sectionValue string local 
	/declare sectionName string local 
	/declare keyName string local
	/declare keyValue string local

	/for i 1 to 64
		/varset e ${${sourceArray}[${i}]}

		/if (${Bool[${e}]}==FALSE) /goto :loopesc

		/varset keyName ${e.Token[1,|]}
		/varset keyValue ${e.Right[-${Math.Calc[${keyName.Length}+1]}]}

		/varset ${destArray}[${i},${bName}] ${keyName}
		/varset ${destArray}[${i},${bValue}] ${keyValue}

		/for j 1 to ${Math.Calc[${keyValue.Count[|]}+1]}
			/varset sectionText ${keyValue.Token[${j},|]}
			/if (${sectionText.Length}==0 || ${Bool[${sectionText}]}==NULL) /goto :loopnexti

			/varset sectionName ${sectionText.Token[1,:]}
			/if (${Select[${sectionName},${autoKeywordIgnoreValues}]}>0) /goto :loopnextj

			|/echo sectionName:${sectionName}
			|/if (${Defined[b${sectionName}]}==FALSE) {
			|	/declare b${sectionName} int outer ${autoKeywordIndex}
			|	/echo /declare b${sectionName} int outer ${autoKeywordIndex}
			|	/varset autoKeywordIndex ${Math.Calc[${autoKeywordIndex}+1]}
			|}

			/if (${sectionText.Count[:]}>0) {
				/varset sectionValue ${sectionText.Token[2,:]}
			} else {
				/varset sectionValue ${sectionText}
			}
			
			/if (${Defined[b${sectionName}]}==FALSE) {
				/bc Warning: >>> ${sectionName} <<< is not a recognized parameter.
				/beep
				/beep
				/continue
			}
			
			/varset ${destArray}[${i},${b${sectionName}}] ${sectionValue}

			:loopnextj
		/next j

		:loopnexti
	/next i
	:loopesc
/RETURN

SUB bandolierProfileIsActive(string profile)
	/declare pri int local ${Me.Inventory[13].ID}
	/declare sec int local ${Me.Inventory[14].ID}
	/declare rng int local ${Me.Inventory[11].ID}
	/declare amm int local ${Me.Inventory[22].ID}

	/if (${Me.Bandolier[${profile}].Item[1].ID} != ${pri}) /return FALSE
	/if (${Me.Bandolier[${profile}].Item[2].ID} != ${sec}) /return FALSE
	/if (${Me.Bandolier[${profile}].Item[3].ID} != ${rng}) /return FALSE
	/if (${Me.Bandolier[${profile}].Item[4].ID} != ${amm}) /return FALSE

	/return TRUE
/RETURN

SUB lockout(string timerName, string dur)
	/if (${Defined[${timerName}]}==TRUE) {
		/varset ${timerName} ${dur}
	} else {
		/declare ${timerName} timer outer ${dur}
	}
/RETURN

SUB isLockoutExpired(string timerName)
	/if (${Defined[${timerName}]}==TRUE) {
		/if (${${timerName}}==0) {
			/deletevar ${timerName}
			/return TRUE
		} else {
			/return FALSE
		}
	}
	/return TRUE
/RETURN

SUB wipeLockout(string timerName)
	/if (${Defined[${timerName}]}==TRUE) {
		/deletevar ${timerName}
	}
/RETURN

SUB isNetBotAttackingID(int targetID)
	/declare netbotCount int local ${NetBots.Counts}
	/declare i int local
	/declare netbotName string local


	/for i 1 to ${netbotCount}
		/varset netbotName ${NetBots.Client[${i}]}	
		/if (${NetBots[${netbotName}].Attacking}==TRUE && ${NetBots[${netbotName}].TargetID}==${targetID}) {
			/return TRUE
		}
	/next i

	/RETURN FALSE
/RETURN

SUB memorizeSpellInGem(string spellName, int gemNumber)
	/if (${Me.Casting.ID}!=NULL) {
		/interrupt
		/delay 5
	}

	/memo "${spellName}" ${gemNumber}

	/delay 3

	/while (${Window[SpellBookWnd]}==TRUE) {
		/delay 3
		/if (${Gem[${spellName}]}!=NULL) /break
	}

	/delay 35 ${Me.SpellReady[${spellName}]}
/RETURN

sub implode(string listName, string delim)
	/if (${delim.Length}==0) /varset delim ,
	/declare i int local
	/declare max int local ${Math.Calc[${${listName}.Count}-1]}
	/if (${max}<0) /return NULL

	/declare r string local

	/for i 0 to ${max}
		/varset r ${r}${delim}${${listName}.Item[${i}]}
	/next i

	/return ${r.Right[-1]}
/return


sub removeToken(string src, string searchVal, string inDelim, string outDelim)
	/if (${inDelim.Length}==0) /varset delim ,
	/if (${outDelim.Length}==0) /varset outDelim ${inDelim}

	/declare i int local
	/declare r string local

	/for i 1 to ${Math.Calc[${src.Count[${inDelim}]}+1]}
		/if (${src.Token[${i},${inDelim}].Equal[${searchVal}]}) /continue
		/varset r ${r}${outDelim}${src.Token[${i},${inDelim}]}
	/next i

	/return ${r.Right[-1]}
/return


#event cleanIniFile "[#*#] e3 clean ini"
#event cleanIniFile "<#*#> e3 clean ini"
sub EVENT_cleanIniFile
   /echo Backfilling INI file with missing key/values...

   /call assist_CharacterSettings TRUE
   /call heals_CharacterSettings TRUE
   /call buffCheck_CharacterSettings TRUE
   /call basics_CharacterSettings TRUE
/return
