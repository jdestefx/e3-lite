| CALLED WHENEVER A CHANTER CLASS STARTS E3

SUB ENC_Setup
/RETURN


SUB ENC_CharacterSettings
	/call WriteToIni "${iniFileCharacter},AssistSpells,Shallow Breath" SelfOverMana:50"
	/call WriteToIni "${iniFileCharacter},AssistSpells,Burst of Flame" "SelfOverMana:50"
/RETURN


SUB ENC_Aliases
/RETURN

SUB ENC_Background_Events
/RETURN
