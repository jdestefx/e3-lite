
| CALLED WHENEVER A CLERIC CLASS STARTS E3
SUB CLR_Setup
/RETURN

| CALLED TO CREATE CLERIC BOT INI ENTRIES
SUB CLR_CharacterSettings
	/call WriteToIni "${iniFileCharacter},Buffs,Courage" "SelfOverMana:70"
	/call WriteToIni "${iniFileCharacter},Heals,Minor Healing" "HealPct:50"
/RETURN

| CREATE CLERIC ALIASES ON INITIAL SETUP
SUB CLR_Aliases

/RETURN


| ROUTINE CLERIC FUNCTIONS
SUB CLR_Background_Events

/RETURN



