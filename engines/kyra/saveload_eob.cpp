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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifdef ENABLE_EOB

#include "kyra/resource.h"
#include "kyra/script_eob.h"

#include "common/system.h"
#include "common/savefile.h"
#include "common/substream.h"

namespace Kyra {

Common::Error EobCoreEngine::loadGameState(int slot) {
	const char *fileName = 0;

	if (slot == -1) {
		_savegameFilename = /*_targetName + */Common::String("eob.fin");
		fileName = _savegameFilename.c_str();
	} else {
		fileName = getSavegameFilename(slot);
	}

	SaveHeader header;
	Common::InSaveFile *saveFile = openSaveForReading(fileName, header);
	if (!saveFile)
		return Common::Error(Common::kReadingFailed);

	Common::SeekableSubReadStreamEndian in(saveFile, saveFile->pos(), saveFile->size(), !header.originalSave, DisposeAfterUse::YES);
	_loading = true;
	_screen->fadeToBlack(10);

	for (int i = 0; i < 6; i++) {
		EobCharacter *c = &_characters[i];
		c->id = in.readByte();
		c->flags = in.readByte();
		in.read(c->name, 11);
		c->strengthCur = in.readSByte();
		c->strengthMax = in.readSByte();
		c->strengthExtCur = in.readSByte();
		c->strengthExtMax = in.readSByte();
		c->intelligenceCur = in.readSByte();
		c->intelligenceMax = in.readSByte();
		c->wisdomCur = in.readSByte();
		c->wisdomMax = in.readSByte();
		c->dexterityCur = in.readSByte();
		c->dexterityMax = in.readSByte();
		c->constitutionCur = in.readSByte();
		c->constitutionMax = in.readSByte();
		c->charismaCur = in.readSByte();
		c->charismaMax = in.readSByte();
		c->hitPointsCur = in.readSint16BE();
		c->hitPointsMax = in.readSint16BE();
		c->armorClass = in.readSByte();
		c->disabledSlots = in.readByte();
		c->raceSex = in.readByte();
		c->cClass = in.readByte();
		c->alignment = in.readByte();
		c->portrait = in.readSByte();
		c->food = in.readByte();
		in.read(c->level, 3);
		for (int ii = 0; ii < 3; ii++)
			c->experience[ii] = in.readUint32BE();
		delete[] c->faceShape;
		c->faceShape = 0;
		in.read(c->mageSpells, 80);
		in.read(c->clericSpells, 80);
		c->mageSpellsAvailableFlags = in.readUint32BE();
		for (int ii = 0; ii < 27; ii++)
			c->inventory[ii] = in.readSint16BE();
		uint32 ct = _system->getMillis();
		for (int ii = 0; ii < 10; ii++) {
			c->timers[ii] = in.readUint32BE();
			if (c->timers[ii])
				c->timers[ii] += ct;
		}
		in.read(c->events, 10);
		in.read(c->effectsRemainder, 4);
		c->effectFlags = in.readUint32BE();
		c->damageTaken = in.readByte();
		in.read(c->slotStatus, 5);
	}

	setupCharacterTimers();

	_screen->loadShapeSetBitmap("CHARGENA", 3, 3);
	for (int i = 0; i < 6; i++) {
		EobCharacter *c = &_characters[i];
		if (!c->flags || c->portrait < 0)
			continue;
		c->faceShape = _screen->encodeShape((c->portrait % 10) << 2, (c->portrait / 10) << 5, 4, 32, true);
	}

	_screen->loadShapeSetBitmap(_flags.gameID == GI_EOB2 ? "OUTPORTS" : "OUTTAKE", 3, 3);
	for (int i = 0; i < 6; i++) {
		EobCharacter *c = &_characters[i];
		if (!c->flags || c->portrait >= 0)
			continue;
		c->faceShape = _screen->encodeShape((-(c->portrait + 1)) << 2, _flags.gameID == GI_EOB2 ? 0 : 160, 4, 32, true);
	}
	_screen->_curPage = 0;

	_currentLevel = in.readByte();
	_currentSub = in.readSByte();
	_currentBlock = in.readUint16BE();
	_currentDirection = in.readUint16BE();
	_itemInHand = in.readSint16BE();
	_hasTempDataFlags = in.readUint32BE();
	_partyEffectFlags = in.readUint32BE();

	_updateFlags = in.readUint16BE();
	_compassDirection = in.readUint16BE();
	_currentControlMode = in.readUint16BE();
	_updateCharNum = in.readUint16BE();
	_openBookSpellLevel = in.readSByte();
	_openBookSpellSelectedItem  = in.readSByte();
	_openBookSpellListOffset = in.readSByte();
	_openBookChar = in.readByte();
	_openBookType = in.readByte();
	_openBookCharBackup = in.readByte();
	_openBookTypeBackup = in.readByte();
	_activeSpellCharId = in.readByte();
	_activeSpellCharacterPos = in.readByte();
	_activeSpell = in.readByte();
	_returnAfterSpellCallback = in.readByte() ? true : false;

	_inf->loadState(in);

	for (int i = 0; i < 600; i++) {
		EobItem *t = &_items[i];
		t->nameUnid = in.readByte();
		t->nameId = in.readByte();
		t->flags = in.readByte();
		t->icon = in.readSByte();
		t->type = in.readSByte();
		t->pos = in.readSByte();
		t->block = in.readSint16BE();
		t->next = in.readSint16BE();
		t->prev = in.readSint16BE();
		t->level = in.readByte();
		t->value = in.readSByte();
	}

	for (int i = 51; i < 65; i++) {
		EobItemType *t = &_itemTypes[i];
		t->invFlags = in.readUint16BE();
		t->handFlags = in.readUint16BE();
		t->armorClass = in.readSByte();
		t->allowedClasses = in.readSByte();
		t->requiredHands = in.readSByte();
		t->dmgNumDiceS = in.readSByte();
		t->dmgNumPipsS = in.readSByte();
		t->dmgIncS = in.readSByte();
		t->dmgNumDiceL = in.readSByte();
		t->dmgNumPipsL = in.readSByte();
		t->dmgIncL = in.readSByte();
		t->unk1 = in.readByte();
		t->extraProperties = in.readUint16BE();
	}

	for (int i = 0; i < 18; i++) {
		if (!(_hasTempDataFlags & (1 << i)))
			continue;

		if (_lvlTempData[i]) {
			delete[] _lvlTempData[i]->wallsXorData;
			delete[] _lvlTempData[i]->flags;
			releaseMonsterTempData(_lvlTempData[i]);
			releaseFlyingObjectTempData(_lvlTempData[i]);
			releaseWallOfForceTempData(_lvlTempData[i]);
			delete _lvlTempData[i];
		}

		_lvlTempData[i] = new LevelTempData;
		LevelTempData *l = _lvlTempData[i];
		l->wallsXorData = new uint8[4096];
		l->flags = new uint16[1024];
		EobMonsterInPlay *lm = new EobMonsterInPlay[30];
		l->monsters = lm;
		EobFlyingObject *lf = new EobFlyingObject[_numFlyingObjects];
		l->flyingObjects = lf;
		WallOfForce *lw = new WallOfForce[5];
		l->wallsOfForce = lw;

		in.read(l->wallsXorData, 4096);
		for (int ii = 0; ii < 1024; ii++)
			l->flags[ii] = in.readByte();

		for (int ii = 0; ii < 30; ii++) {
			EobMonsterInPlay *m = &lm[ii];
			m->type = in.readByte();
			m->unit = in.readByte();
			m->block = in.readUint16BE();
			m->pos = in.readByte();
			m->dir = in.readSByte();
			m->animStep = in.readByte();
			m->shpIndex = in.readByte();
			m->mode = in.readSByte();
			m->f_9 = in.readSByte();
			m->curAttackFrame = in.readSByte();
			m->spellStatusLeft = in.readSByte();
			m->hitPointsMax = in.readSint16BE();
			m->hitPointsCur = in.readSint16BE();
			m->dest = in.readUint16BE();
			m->randItem = in.readUint16BE();
			m->fixedItem = in.readUint16BE();
			m->flags = in.readByte();
			m->idleAnimState = in.readByte();
			m->curRemoteWeapon = in.readByte();
			m->numRemoteAttacks = in.readByte();
			m->palette = in.readSByte();
			m->directionChanged = in.readByte();
			m->stepsTillRemoteAttack = in.readByte();
			m->sub = in.readByte();
		}

		for (int ii = 0; ii < _numFlyingObjects; ii++) {
			EobFlyingObject *m = &lf[ii];
			m->enable = in.readByte();
			m->objectType = in.readByte();
			m->attackerId = in.readSint16BE();
			m->item = in.readSint16BE();
			m->curBlock = in.readUint16BE();
			m->u2 = in.readUint16BE();
			m->u1 = in.readByte();
			m->direction = in.readByte();
			m->distance = in.readByte();
			m->callBackIndex = in.readSByte();
			m->curPos = in.readByte();
			m->flags = in.readByte();
			m->unused = in.readByte();
		}

		for (int ii = 0; ii < 5; ii++) {
			WallOfForce *w = &lw[ii];
			w->block = in.readUint16BE();
			w->duration = in.readUint32BE();
		}
	}

	loadLevel(_currentLevel, _currentSub);
	_sceneUpdateRequired = true;
	_screen->setFont(Screen::FID_6_FNT);

	for (int i = 0; i < 6; i++) {
		for (int ii = 0; ii < 10; ii++) {
			if (_characters[i].events[ii] == -57)
				spellCallback_start_trueSeeing();
		}
	}

	_screen->setCurPage(0);
	gui_drawPlayField(false);

	if (_currentControlMode)
		_screen->copyRegion(176, 0, 0, 0, 144, 168, 0, 5, Screen::CR_NO_P_CHECK);

	_screen->setCurPage(0);
	gui_drawAllCharPortraitsWithStats();
	drawScene(1);

	if (_updateFlags) {
		_updateFlags = 0;
		useMagicBookOrSymbol(_openBookChar, _openBookType);
	}

	_screen->copyRegion(0, 120, 0, 0, 176, 24, 0, 12, Screen::CR_NO_P_CHECK);

	gui_toggleButtons();
	setHandItem(_itemInHand);

	while (!_screen->isMouseVisible())
		_screen->showMouse();

	_loading = false;
	_screen->fadeFromBlack(20);
	removeInputTop();

	return Common::kNoError;
}

Common::Error EobCoreEngine::saveGameStateIntern(int slot, const char *saveName, const Graphics::Surface *thumbnail) {
	Common::String saveNameTmp;
	const char *fileName = 0;

	if (slot == -1) {
		_savegameFilename = _targetName + Common::String(".fin");
		fileName = _savegameFilename.c_str();
		saveNameTmp = _targetName + Common::String(" final");
		saveNameTmp.toUppercase();
		saveName = saveNameTmp.c_str();
	} else {
		fileName = getSavegameFilename(slot);
	}

	Common::OutSaveFile *out = openSaveForWriting(fileName, saveName, thumbnail);
	if (!out)
		return _saveFileMan->getError();

	completeDoorOperations();
	generateTempData();
	advanceTimers(_restPartyElapsedTime);
	_restPartyElapsedTime = 0;

	for (int i = 0; i < 6; i++)
		timerSpecialCharacterUpdate(0x30 + i);

	for (int i = 0; i < 6; i++) {
		EobCharacter *c = &_characters[i];

		out->writeByte(c->id);
		out->writeByte(c->flags);
		out->write(c->name, 11);
		out->writeSByte(c->strengthCur);
		out->writeSByte(c->strengthMax);
		out->writeSByte(c->strengthExtCur);
		out->writeSByte(c->strengthExtMax);
		out->writeSByte(c->intelligenceCur);
		out->writeSByte(c->intelligenceMax);
		out->writeSByte(c->wisdomCur);
		out->writeSByte(c->wisdomMax);
		out->writeSByte(c->dexterityCur);
		out->writeSByte(c->dexterityMax);
		out->writeSByte(c->constitutionCur);
		out->writeSByte(c->constitutionMax);
		out->writeSByte(c->charismaCur);
		out->writeSByte(c->charismaMax);
		out->writeSint16BE(c->hitPointsCur);
		out->writeSint16BE(c->hitPointsMax);
		out->writeSByte(c->armorClass);
		out->writeByte(c->disabledSlots);
		out->writeByte(c->raceSex);
		out->writeByte(c->cClass);
		out->writeByte(c->alignment);
		out->writeByte(c->portrait);
		out->writeByte(c->food);
		out->write(c->level, 3);
		for (int ii = 0; ii < 3; ii++)
			out->writeUint32BE(c->experience[ii]);
		out->write(c->mageSpells, 80);
		out->write(c->clericSpells, 80);
		out->writeUint32BE(c->mageSpellsAvailableFlags);
		for (int ii = 0; ii < 27; ii++)
			out->writeSint16BE(c->inventory[ii]);
		uint32 ct = _system->getMillis();
		for (int ii = 0; ii < 10; ii++)
			out->writeUint32BE((c->timers[ii] && c->timers[ii] > ct) ? c->timers[ii] - ct : 0);

		out->write(c->events, 10);
		out->write(c->effectsRemainder, 4);
		out->writeUint32BE(c->effectFlags);
		out->writeByte(c->damageTaken);
		out->write(c->slotStatus, 5);
	}

	out->writeByte(_currentLevel);
	out->writeSByte(_currentSub);
	out->writeUint16BE(_currentBlock);
	out->writeUint16BE(_currentDirection);
	out->writeSint16BE(_itemInHand);
	out->writeUint32BE(_hasTempDataFlags);
	out->writeUint32BE(_partyEffectFlags);

	out->writeUint16BE(_updateFlags);
	out->writeUint16BE(_compassDirection);
	out->writeUint16BE(_currentControlMode);
	out->writeUint16BE(_updateCharNum);
	out->writeSByte(_openBookSpellLevel);
	out->writeSByte(_openBookSpellSelectedItem);
	out->writeSByte(_openBookSpellListOffset);
	out->writeByte(_openBookChar);
	out->writeByte(_openBookType);
	out->writeByte(_openBookCharBackup);
	out->writeByte(_openBookTypeBackup);
	out->writeByte(_activeSpellCharId);
	out->writeByte(_activeSpellCharacterPos);
	out->writeByte(_activeSpell);
	out->writeByte(_returnAfterSpellCallback ? 1 : 0);

	_inf->saveState(out);

	for (int i = 0; i < 600; i++) {
		EobItem *t = &_items[i];
		out->writeByte(t->nameUnid);
		out->writeByte(t->nameId);
		out->writeByte(t->flags);
		out->writeSByte(t->icon);
		out->writeSByte(t->type);
		out->writeSByte(t->pos);
		out->writeSint16BE(t->block);
		out->writeSint16BE(t->next);
		out->writeSint16BE(t->prev);
		out->writeByte(t->level);
		out->writeSByte(t->value);
	}

	for (int i = 51; i < 65; i++) {
		EobItemType *t = &_itemTypes[i];
		out->writeUint16BE(t->invFlags);
		out->writeUint16BE(t->handFlags);
		out->writeSByte(t->armorClass);
		out->writeSByte(t->allowedClasses);
		out->writeSByte(t->requiredHands);
		out->writeSByte(t->dmgNumDiceS);
		out->writeSByte(t->dmgNumPipsS);
		out->writeSByte(t->dmgIncS);
		out->writeSByte(t->dmgNumDiceL);
		out->writeSByte(t->dmgNumPipsL);
		out->writeSByte(t->dmgIncL);
		out->writeByte(t->unk1);
		out->writeUint16BE(t->extraProperties);
	}

	for (int i = 0; i < 18; i++) {
		LevelTempData *l = _lvlTempData[i];
		if (!l || !(_hasTempDataFlags & (1 << i)))
			continue;

		out->write(l->wallsXorData, 4096);
		for (int ii = 0; ii < 1024; ii++)
			out->writeByte(l->flags[ii] & 0xff);

		EobMonsterInPlay *lm = (EobMonsterInPlay*)_lvlTempData[i]->monsters;
		EobFlyingObject *lf = (EobFlyingObject*)_lvlTempData[i]->flyingObjects;
		WallOfForce *lw = (WallOfForce*)_lvlTempData[i]->wallsOfForce;

		for (int ii = 0; ii < 30; ii++) {
			EobMonsterInPlay *m = &lm[ii];
			out->writeByte(m->type);
			out->writeByte(m->unit);
			out->writeUint16BE(m->block);
			out->writeByte(m->pos);
			out->writeSByte(m->dir);
			out->writeByte(m->animStep);
			out->writeByte(m->shpIndex);
			out->writeSByte(m->mode);
			out->writeSByte(m->f_9);
			out->writeSByte(m->curAttackFrame);
			out->writeSByte(m->spellStatusLeft);
			out->writeSint16BE(m->hitPointsMax);
			out->writeSint16BE(m->hitPointsCur);
			out->writeUint16BE(m->dest);
			out->writeUint16BE(m->randItem);
			out->writeUint16BE(m->fixedItem);
			out->writeByte(m->flags);
			out->writeByte(m->idleAnimState);
			out->writeByte(m->curRemoteWeapon);
			out->writeByte(m->numRemoteAttacks);
			out->writeSByte(m->palette);
			out->writeByte(m->directionChanged);
			out->writeByte(m->stepsTillRemoteAttack);
			out->writeByte(m->sub);
		}

		for (int ii = 0; ii < _numFlyingObjects; ii++) {
			EobFlyingObject *m = &lf[ii];
			out->writeByte(m->enable);
			out->writeByte(m->objectType);
			out->writeSint16BE(m->attackerId);
			out->writeSint16BE(m->item);
			out->writeUint16BE(m->curBlock);
			out->writeUint16BE(m->u2);
			out->writeByte(m->u1);
			out->writeByte(m->direction);
			out->writeByte(m->distance);
			out->writeSByte(m->callBackIndex);
			out->writeByte(m->curPos);
			out->writeByte(m->flags);
			out->writeByte(m->unused);
		}

		for (int ii = 0; ii < 5; ii++) {
			WallOfForce *w = &lw[ii];
			out->writeUint16BE(w->block);
			out->writeUint32BE(w->duration);
		}
	}

	out->finalize();

	// check for errors
	if (out->err()) {
		warning("Can't write file '%s'. (Disk full?)", fileName);
		return Common::kUnknownError;
	} else {
		debugC(1, kDebugLevelMain, "Saved game '%s.'", saveName);
	}

	delete out;

	_gui->notifyUpdateSaveSlotsList();

	return Common::kNoError;
}

void *EobCoreEngine::generateMonsterTempData(LevelTempData *tmp) {
	EobMonsterInPlay *m = new EobMonsterInPlay[30];
	memcpy(m, _monsters,  sizeof(EobMonsterInPlay) * 30);
	return m;
}

void EobCoreEngine::restoreMonsterTempData(LevelTempData *tmp) {
	memcpy(_monsters, tmp->monsters, sizeof(EobMonsterInPlay) * 30);
}

void EobCoreEngine::releaseMonsterTempData(LevelTempData *tmp) {
	EobMonsterInPlay *p = (EobMonsterInPlay*)tmp->monsters;
	delete[] p;
}

void *EobCoreEngine::generateWallOfForceTempData(LevelTempData *tmp) {
	WallOfForce *w = new WallOfForce[5];
	memcpy(w, _wallsOfForce,  sizeof(WallOfForce) * 5);
	uint32 ct = _system->getMillis();
	for (int i = 0; i < 5; i++)
		w[i].duration = (w[i].duration > ct) ? w[i].duration - ct : _tickLength;
	return w;
}

void EobCoreEngine::restoreWallOfForceTempData(LevelTempData *tmp) {
	memcpy(_wallsOfForce, tmp->wallsOfForce,  sizeof(WallOfForce) * 5);
	uint32 ct = _system->getMillis();
	for (int i = 0; i < 5; i++)
		_wallsOfForce[i].duration += ct;
}

void EobCoreEngine::releaseWallOfForceTempData(LevelTempData *tmp) {
	WallOfForce *p = (WallOfForce*)tmp->wallsOfForce;
	delete[] p;
}

}	// End of namespace Kyra

#endif // ENABLE_EOB
