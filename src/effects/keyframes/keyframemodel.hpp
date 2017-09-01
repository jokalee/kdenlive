/***************************************************************************
 *   Copyright (C) 2017 by Nicolas Carion                                  *
 *   This file is part of Kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3 or any later version accepted by the       *
 *   membership of KDE e.V. (or its successor approved  by the membership  *
 *   of KDE e.V.), which shall act as a proxy defined in Section 14 of     *
 *   version 3 of the license.                                             *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef KEYFRAMELISTMODEL_H
#define KEYFRAMELISTMODEL_H

#include "definitions.h"
#include "gentime.h"
#include "undohelper.hpp"

#include <QAbstractListModel>
#include <QReadWriteLock>

#include <map>
#include <memory>

class DocUndoStack;
class EffectItemModel;

/* @brief This class is the model for a list of keyframes.
   A keyframe is defined by a time, and a type;
   We store them in a sorted fashion using a std::map
 */

enum class KeyframeType
{
    Linear,
    Discrete,
    Curve
};
Q_DECLARE_METATYPE(KeyframeType)
using Keyframe = std::pair<GenTime, std::pair<KeyframeType, double>>;

class KeyframeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /* @brief Construct a keyframe list bound to the given effect
       @param init_value is the value taken by the param at time 0.
     */
    explicit KeyframeModel(double init_value, std::weak_ptr<EffectItemModel> effect, std::weak_ptr<DocUndoStack> undo_stack, QObject *parent = nullptr);

    enum { TypeRole = Qt::UserRole + 1, PosRole, FrameRole, ValueRole};

    /* @brief Adds a keyframe at the given position. If there is already one then we update it.
       @param pos defines the position of the keyframe, relative to the clip
       @param type is the type of the keyframe.
     */
    bool addKeyframe(GenTime pos, KeyframeType type, double value);

protected:
    /* @brief Same function but accumulates undo/redo */
    bool addKeyframe(GenTime pos, KeyframeType type, double value, Fun &undo, Fun &redo);

public:
    /* @brief Removes the keyframe at the given position. */
    bool removeKeyframe(GenTime pos);
    /* @brief Delete all the keyframes of the model */
    bool removeAllKeyframes();

protected:
    /* @brief Same function but accumulates undo/redo */
    bool removeKeyframe(GenTime pos, Fun &undo, Fun &redo);

public:
    /* @brief moves a keyframe
       @param oldPos is the old position of the keyframe
       @param pos defines the new position of the keyframe, relative to the clip
    */
    bool moveKeyframe(GenTime oldPos, GenTime pos);

    /* @brief updates the value of a keyframe
       @param old is the position of the keyframe
       @param value is the new value of the param
    */
    bool updateKeyframe(GenTime pos, double value);

    /* @brief Returns a keyframe data at given pos
       ok is a return parameter, set to true if everything went good
     */
    Keyframe getKeyframe(const GenTime &pos, bool *ok) const;

    /* @brief Returns true if a keyframe exists at given pos
       Notice that add/remove queries are done in real time (gentime), but this request is made in frame
     */
    Q_INVOKABLE bool hasKeyframe(int frame) const;

    /** @brief returns the keyframes as a Mlt Anim Property string.
        It is defined as pairs of frame and value, separated by ;
        Example : "0|=50; 50|=100; 100=200; 200~=60;"
        Spaces are ignored by Mlt.
        |= represents a discrete keyframe, = a linear one and ~= a Catmull-Rom spline
    */
    QString getAnimProperty() const;

    // Mandatory overloads
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

protected:

    /** @brief Helper function that generate a lambda to change type / value of given keyframe */
    Fun updateKeyframe_lambda(GenTime pos, KeyframeType type, double value);

    /** @brief Helper function that generate a lambda to add given keyframe */
    Fun addKeyframe_lambda(GenTime pos, KeyframeType type, double value);

    /** @brief Helper function that generate a lambda to remove given keyframe */
    Fun deleteKeyframe_lambda(GenTime pos);

    /* @brief Connects the signals of this object */
    void setup();
private:

    std::weak_ptr<EffectItemModel> m_effect;
    std::weak_ptr<DocUndoStack> m_undoStack;

    mutable QReadWriteLock m_lock; // This is a lock that ensures safety in case of concurrent access

    std::map<GenTime, std::pair<KeyframeType, double>> m_keyframeList;

signals:
    void modelChanged();

public:
    // this is to enable for range loops
    auto begin() -> decltype(m_keyframeList.begin()) { return m_keyframeList.begin(); }
    auto end() -> decltype(m_keyframeList.end()) { return m_keyframeList.end(); }
};
//Q_DECLARE_METATYPE(KeyframeModel *)

#endif