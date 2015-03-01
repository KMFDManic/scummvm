/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "xeen/spells.h"
#include "xeen/dialogs_spells.h"
#include "xeen/files.h"
#include "xeen/resources.h"
#include "xeen/xeen.h"

namespace Xeen {

Spells::Spells(XeenEngine *vm) : _vm(vm) {
	_lastCaster = 0;

	load();
}

void Spells::load() {
	File f1("spells.xen");
	while (f1.pos() < f1.size())
		_spellNames.push_back(f1.readString());
	f1.close();
}

int Spells::calcSpellCost(int spellId, int expenseFactor) const {
	int amount = SPELL_COSTS[spellId];
	return (amount >= 0) ? (amount * 100) << expenseFactor :
		(amount * -500) << expenseFactor;
}

int Spells::calcSpellPoints(int spellId, int expenseFactor) const {
	int amount = SPELL_COSTS[spellId];
	return (amount >= 0) ? amount : amount * -1 * expenseFactor;
}

typedef void(Spells::*SpellMethodPtr)();

void Spells::executeSpell(int spellId) {
	static const SpellMethodPtr SPELL_LIST[73] = {
		&Spells::light, &Spells::awaken, &Spells::magicArrow, &Spells::firstAid,
		&Spells::flyingFist, &Spells::energyBlast, &Spells::sleep,
		&Spells::revitalize, &Spells::cureWounds, &Spells::sparks,

		&Spells::shrapMetal, &Spells::insectSpray, &Spells::toxicCloud,
		&Spells::protectionFromElements, &Spells::pain, &Spells::jump,
		&Spells::beastMaster, &Spells::clairvoyance, &Spells::turnUndead,
		&Spells::levitate,

		&Spells::wizardEye, &Spells::bless, &Spells::identifyMonster,
		&Spells::lightningBolt, &Spells::holyBonus, &Spells::powerCure,
		&Spells::naturesCure, &Spells::lloydsBeacon, &Spells::powerShield,
		&Spells::heroism,

		&Spells::hypnotize, &Spells::walkOnWater, &Spells::frostByte,
		&Spells::detectMonster, &Spells::fireball, &Spells::coldRay,
		&Spells::curePoison, &Spells::acidSpray, &Spells::timeDistortion,
		&Spells::dragonSleep,

		&Spells::suppressPoison, &Spells::teleport, &Spells::fingerOfDeath,
		&Spells::cureParalysis, &Spells::golemStopper, &Spells::poisonVolley,
		&Spells::deadlySwarm, &Spells::superShelter, &Spells::dayOfProtection,
		&Spells::dayOfSorcery,

		&Spells::createFood, &Spells::fieryFlail, &Spells::rechargeItem,
		&Spells::fantasticFreeze, &Spells::townPortal, &Spells::stoneToFlesh,
		&Spells::raiseDead, &Spells::etherialize, &Spells::dancingSword,
		&Spells::moonRay,

		&Spells::massDistortion, &Spells::prismaticLight, &Spells::enchantItem,
		&Spells::incinerate, &Spells::holyWord, &Spells::resurrection,
		&Spells::elementalStorm, &Spells::megaVolts, &Spells::inferno,
		&Spells::sunRay,

		&Spells::implosion, &Spells::starBurst, &Spells::divineIntervention
	};

	(this->*SPELL_LIST[spellId])();
}

/**
 * Spell being cast failed
 */
void Spells::spellFailed() {
	ErrorScroll::show(_vm, SPELL_FAILED, WT_NONFREEZED_WAIT);
}

/**
 * Cast a spell associated with an item
 */
void Spells::castItemSpell(int spellId) {
	if (_vm->_mode == MODE_COMBAT) {
		if (spellId == 15 || spellId == 20 || spellId == 27 || spellId == 41
				|| spellId == 47 || spellId == 54 || spellId == 57) {
			ErrorDialog::show(_vm, Common::String::format(CANT_CAST_WHILE_ENGAGED,
				_spellNames[spellId].c_str()));
			return;
		}
	}

	executeSpell(spellId);
}

/**
 * Cast a given spell
 */
int Spells::castSpell(Character *c, int spellId) {
	Combat &combat = *_vm->_combat;
	Interface &intf = *_vm->_interface;
	int oldTillMove = intf._tillMove;
	int result = 1;
	combat._oldCharacter = c;
	
	// Try and subtract the SP and gem requirements for the spell
	int resultError = subSpellCost(*c, spellId);
	if (resultError) {
		CantCast::show(_vm, spellId, resultError);
		result = -1;
	} else {
		// Some spells have special handling
		switch (spellId) {
		case 19:	// Enchant item
		case 21:	// Etherialize
		case 40:	// Jump
		case 44:	// Lloyd's Beacon
		case 66:	// Super Shelter
		case 69:	// Teleport
		case 71:	// Town Portal
		case 75:	// Wizard Eye
			if (_vm->_mode != MODE_COMBAT) {
				executeSpell(spellId);
			} else {
				// Return the spell costs and flag that another spell can be selected
				addSpellCost(*c, spellId);
				ErrorDialog::show(_vm, Common::String::format(CANT_CAST_WHILE_ENGAGED,
					_spellNames[spellId].c_str()));
				result = -1;
			}
			break;

		default:
			executeSpell(spellId);
			break;
		}
	}

	combat._moveMonsters = 1;
	intf._tillMove = oldTillMove;
	return result;
}

/**
 * Subtract the requirements for a given spell if available, returning
 * true if there was sufficient
 */
int Spells::subSpellCost(Character &c, int spellId) {
	Party &party = *_vm->_party;
	int gemCost = SPELL_GEM_COST[spellId];
	int spCost = SPELL_COSTS[spellId];

	// Negative SP costs indicate it's dependent on the character's level
	if (spCost <= 0) {
		spCost = c.getCurrentLevel() * (spCost * -1);
	}

	if (spCost > c._currentSp)
		// Not enough SP
		return 1;
	if (gemCost > (int)party._gems)
		// Not enough gems
		return 2;

	c._currentSp -= spCost;
	party._gems -= gemCost;
	return 0;
}

/**
 * Add the SP and gem requirements for a given spell to the given 
 * character and party
 */
void Spells::addSpellCost(Character &c, int spellId) {
	Party &party = *_vm->_party;
	int gemCost = SPELL_GEM_COST[spellId];
	int spCost = SPELL_COSTS[spellId];

	if (spCost < 1)
		spCost *= -1 * c.getCurrentLevel();

	c._currentSp += spCost;
	party._gems += gemCost;
}

void Spells::light() { 
	Interface &intf = *_vm->_interface;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	++party._lightCount;
	if (intf._intrIndex1)
		party._stepped = true;
	sound.playFX(39);
}

void Spells::awaken() { 
	Interface &intf = *_vm->_interface;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	for (uint idx = 0; idx < party._activeParty.size(); ++idx) {
		Character &c = party._activeParty[idx];
		c._conditions[ASLEEP] = 0;
		if (c._currentHp > 0)
			c._conditions[UNCONSCIOUS] = 0;
	}

	intf.drawParty(true);
	sound.playFX(30);
}

void Spells::magicArrow() { 
	Combat &combat = *_vm->_combat;
	combat._monsterDamage = 0;
	combat._damageType = DT_MAGIC_ARROW;
	combat._rangeType = RT_SINGLE;
	combat.multiAttack(11);
}

void Spells::firstAid() {
	Combat &combat = *_vm->_combat;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	int charIndex = SpellOnWho::show(_vm, MS_FirstAid);
	if (charIndex == -1)
		return;

	Character &c = combat._combatMode == 2 ? *combat._combatParty[charIndex] :
		party._activeParty[charIndex];

	if (c.isDead()) {
		spellFailed();
	} else {
		sound.playFX(30);
		c.addHitPoints(6);
	}
}

void Spells::flyingFist() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = 6;
	combat._damageType = DT_PHYSICAL;
	combat._rangeType = RT_SINGLE;
	sound.playFX(18);
	combat.multiAttack(14);
}

void Spells::energyBlast() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = combat._oldCharacter->getCurrentLevel() * 2;
	combat._damageType = DT_ENERGY;
	combat._rangeType = RT_SINGLE;
	sound.playFX(16);
	combat.multiAttack(13);
}

void Spells::sleep() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = 0;
	combat._damageType = DT_SLEEP;
	combat._rangeType = RT_GROUP;
	sound.playFX(18);
	combat.multiAttack(7);
}

void Spells::revitalize() {
	Combat &combat = *_vm->_combat;
	Interface &intf = *_vm->_interface;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	int charIndex = SpellOnWho::show(_vm, MS_Revitalize);
	if (charIndex == -1)
		return;

	Character &c = combat._combatMode == 2 ? *combat._combatParty[charIndex] :
		party._activeParty[charIndex];

	sound.playFX(30);
	c.addHitPoints(0);
	c._conditions[WEAK] = 0;
	intf.drawParty(true);
}

void Spells::cureWounds() {
	Combat &combat = *_vm->_combat;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	int charIndex = SpellOnWho::show(_vm, MS_Revitalize);
	if (charIndex == -1)
		return;

	Character &c = combat._combatMode == 2 ? *combat._combatParty[charIndex] :
		party._activeParty[charIndex];

	if (c.isDead()) {
		spellFailed();
	} else {
		sound.playFX(30);
		c.addHitPoints(15);
	}
}

void Spells::sparks() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = combat._oldCharacter->getCurrentLevel() * 2;
	combat._damageType = DT_ELECTRICAL;
	combat._rangeType = RT_GROUP;
	sound.playFX(14);
	combat.multiAttack(5);
}

void Spells::shrapMetal() { error("TODO: spell"); }
void Spells::insectSpray() { error("TODO: spell"); }

void Spells::toxicCloud() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = 10;
	combat._damageType = DT_POISON;
	combat._rangeType = RT_GROUP;
	sound.playFX(17);
	combat.multiAttack(10);
}

void Spells::suppressPoison() {
	Combat &combat = *_vm->_combat;
	Interface &intf = *_vm->_interface;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;

	int charIndex = SpellOnWho::show(_vm, MS_FirstAid);
	if (charIndex == -1)
		return;

	Character &c = combat._combatMode == 2 ? *combat._combatParty[charIndex] :
		party._activeParty[charIndex];

	if (c._conditions[POISONED]) {
		if (c._conditions[POISONED] >= 4) {
			c._conditions[POISONED] -= 2;
		} else {
			c._conditions[POISONED] = 1;
		}
	}

	sound.playFX(20);
	c.addHitPoints(0);
	intf.drawParty(1);
}

void Spells::protectionFromElements() {
	Combat &combat = *_vm->_combat;
	Interface &intf = *_vm->_interface;
	Party &party = *_vm->_party;
	SoundManager &sound = *_vm->_sound;
	Character &c = *combat._oldCharacter;
	int resist = MIN(c.getCurrentLevel() * 2 + 5, (uint)200);
	
	int elementType = SelectElement::show(_vm, MS_ProtFromElements);
	if (elementType != -1) {
		switch (elementType) {
		case DT_FIRE:
			party._fireResistence = resist;
			break;
		case DT_ELECTRICAL:
			party._fireResistence = resist;
			break;
		case DT_COLD:
			party._coldResistence = resist;
			break;
		case DT_POISON:
			party._poisonResistence = resist;
			break;
		default:
			break;
		}

		sound.playFX(20);
		intf.drawParty(true);
	}
}

void Spells::pain() {
	Combat &combat = *_vm->_combat;
	SoundManager &sound = *_vm->_sound;

	combat._monsterDamage = 0;
	combat._damageType = DT_PHYSICAL;
	combat._rangeType = RT_GROUP;
	sound.playFX(18);
	combat.multiAttack(14);
}

void Spells::jump() { error("TODO: spell"); }			// Not while engaged
void Spells::beastMaster() { error("TODO: spell"); }
void Spells::clairvoyance() { error("TODO: spell"); }
void Spells::turnUndead() { error("TODO: spell"); }
void Spells::levitate() { error("TODO: spell"); }

void Spells::wizardEye() { error("TODO: spell"); }		// Not while engaged
void Spells::bless() { error("TODO: spell"); }
void Spells::identifyMonster() { error("TODO: spell"); }
void Spells::lightningBolt() { error("TODO: spell"); }
void Spells::holyBonus() { error("TODO: spell"); }
void Spells::powerCure() { error("TODO: spell"); }
void Spells::naturesCure() { error("TODO: spell"); }
void Spells::lloydsBeacon() { error("TODO: spell"); }	// Not while engaged
void Spells::powerShield() { error("TODO: spell"); }
void Spells::heroism() { error("TODO: spell"); }

void Spells::hypnotize() { error("TODO: spell"); }
void Spells::walkOnWater() { error("TODO: spell"); }
void Spells::frostByte() { error("TODO: spell"); }
void Spells::detectMonster() { error("TODO: spell"); }
void Spells::fireball() { error("TODO: spell"); }
void Spells::coldRay() { error("TODO: spell"); }
void Spells::curePoison() { error("TODO: spell"); }
void Spells::acidSpray() { error("TODO: spell"); }
void Spells::timeDistortion() { error("TODO: spell"); }
void Spells::dragonSleep() { error("TODO: spell"); }

void Spells::teleport() { error("TODO: spell"); }		// Not while engaged
void Spells:: fingerOfDeath() { error("TODO: spell"); }
void Spells::cureParalysis() { error("TODO: spell"); }
void Spells::golemStopper() { error("TODO: spell"); }
void Spells::poisonVolley() { error("TODO: spell"); }
void Spells::deadlySwarm() { error("TODO: spell"); }
void Spells::superShelter() { error("TODO: spell"); }	// Not while engaged
void Spells::dayOfProtection() { error("TODO: spell"); }
void Spells::dayOfSorcery() { error("TODO: spell"); }

void Spells::createFood() { error("TODO: spell"); }
void Spells::fieryFlail() { error("TODO: spell"); }
void Spells::rechargeItem() { error("TODO: spell"); }
void Spells::fantasticFreeze() { error("TODO: spell"); }
void Spells::townPortal() { error("TODO: spell"); }		// Not while engaged
void Spells::stoneToFlesh() { error("TODO: spell"); }
void Spells::raiseDead() { error("TODO: spell"); }
void Spells::etherialize() { error("TODO: spell"); }		// Not while engaged
void Spells::dancingSword() { error("TODO: spell"); }
void Spells::moonRay() { error("TODO: spell"); }

void Spells::massDistortion() { error("TODO: spell"); }
void Spells::prismaticLight() { error("TODO: spell"); }
void Spells::enchantItem() { error("TODO: spell"); }
void Spells::incinerate() { error("TODO: spell"); }
void Spells::holyWord() { error("TODO: spell"); }
void Spells::resurrection() { error("TODO: spell"); }
void Spells::elementalStorm() { error("TODO: spell"); }
void Spells::megaVolts() { error("TODO: spell"); }
void Spells::inferno() { error("TODO: spell"); }
void Spells::sunRay() { error("TODO: spell"); }

void Spells::implosion() { error("TODO: spell"); }
void Spells::starBurst() { error("TODO: spell"); }
void Spells::divineIntervention() { error("TODO: spell"); }

} // End of namespace Xeen
