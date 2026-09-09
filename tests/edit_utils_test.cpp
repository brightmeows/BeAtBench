// SPDX-License-Identifier: GPL-3.0-only
// EditUtils 单测（2026-09）：SessionController 纯函数回抽后的行为等价 + Base62 文本判定。
// 只链 Qt6::Core（无 core 模型 / GUI）。
#include <gtest/gtest.h>

#include <QVariantMap>

#include "bridge/EditUtils.hpp"

using beatbench::app::EditUtils;

namespace {

QVariantMap makeRef(int measure, int num, int den, int sample, const QString& kind, int index,
                    int player, QVariant subLine = QVariant()) {
    QVariantMap lane;
    lane.insert(QStringLiteral("kind"), kind);
    lane.insert(QStringLiteral("index"), index);
    lane.insert(QStringLiteral("player"), player);
    QVariantMap pos;
    pos.insert(QStringLiteral("num"), num);
    pos.insert(QStringLiteral("den"), den);
    QVariantMap r;
    r.insert(QStringLiteral("measure"), measure);
    r.insert(QStringLiteral("sample"), sample);
    r.insert(QStringLiteral("lane"), lane);
    r.insert(QStringLiteral("pos"), pos);
    if (subLine.isValid()) r.insert(QStringLiteral("sub_line"), subLine);
    return r;
}

QVariantMap makeDelta(int measure, int num, int den) {
    QVariantMap pos;
    pos.insert(QStringLiteral("num"), num);
    pos.insert(QStringLiteral("den"), den);
    QVariantMap d;
    d.insert(QStringLiteral("measure"), measure);
    d.insert(QStringLiteral("pos"), pos);
    return d;
}

}  // namespace

TEST(EditUtils, Gcd) {
    EditUtils u;
    EXPECT_EQ(u.gcd(12, 18), 6);
    EXPECT_EQ(u.gcd(-12, 18), 6);
    EXPECT_EQ(u.gcd(5, 0), 5);
    EXPECT_EQ(u.gcd(0, 7), 7);
    EXPECT_EQ(u.gcd(0, 0), 1);  // 旧 JS：0 → 1（避免除零）
    EXPECT_EQ(u.gcd(7, 13), 1);
}

TEST(EditUtils, RefEquals) {
    EditUtils u;
    const auto a = makeRef(3, 1, 4, 7, "key", 2, 1, 0);
    EXPECT_TRUE(u.refEquals(a, a));
    EXPECT_TRUE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 2, 1, 0)));
    EXPECT_FALSE(u.refEquals(a, makeRef(4, 1, 4, 7, "key", 2, 1, 0)));      // measure
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 1, 4, 8, "key", 2, 1, 0)));      // sample
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 1, 4, 7, "scratch", 2, 1, 0)));  // lane.kind
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 3, 1, 0)));      // lane.index
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 2, 2, 0)));      // lane.player
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 2, 8, 7, "key", 2, 1, 0)));      // pos（未约分也不同）
    // sub_line：任一侧缺失 = 通配；两侧都有则必须相等
    EXPECT_TRUE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 2, 1)));
    EXPECT_TRUE(u.refEquals(makeRef(3, 1, 4, 7, "key", 2, 1), a));
    EXPECT_FALSE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 2, 1, 1)));
    EXPECT_TRUE(u.refEquals(a, makeRef(3, 1, 4, 7, "key", 2, 1, 0)));
    // 空 map（null ref）→ false
    EXPECT_FALSE(u.refEquals(QVariantMap(), a));
    EXPECT_FALSE(u.refEquals(a, QVariantMap()));
}

TEST(EditUtils, AddPosDelta) {
    EditUtils u;
    // 1/4 + 1/4 = 1/2（同小节）
    const auto r1 = u.addPosDelta(makeRef(0, 1, 4, 1, "key", 0, 1), makeDelta(0, 1, 4));
    EXPECT_EQ(r1.value(QStringLiteral("measure")).toInt(), 0);
    EXPECT_EQ(r1.value(QStringLiteral("pos")).toMap().value(QStringLiteral("num")).toInt(), 1);
    EXPECT_EQ(r1.value(QStringLiteral("pos")).toMap().value(QStringLiteral("den")).toInt(), 2);
    // 3/4 + 1/2 = 5/4 → 进位到下一小节 1/4
    const auto r2 = u.addPosDelta(makeRef(0, 3, 4, 1, "key", 0, 1), makeDelta(0, 1, 2));
    EXPECT_EQ(r2.value(QStringLiteral("measure")).toInt(), 1);
    EXPECT_EQ(r2.value(QStringLiteral("pos")).toMap().value(QStringLiteral("num")).toInt(), 1);
    EXPECT_EQ(r2.value(QStringLiteral("pos")).toMap().value(QStringLiteral("den")).toInt(), 4);
    // 约分：1/6 + 1/6 = 2/6 → 1/3
    const auto r3 = u.addPosDelta(makeRef(2, 1, 6, 1, "key", 0, 1), makeDelta(0, 1, 6));
    EXPECT_EQ(r3.value(QStringLiteral("measure")).toInt(), 2);
    EXPECT_EQ(r3.value(QStringLiteral("pos")).toMap().value(QStringLiteral("num")).toInt(), 1);
    EXPECT_EQ(r3.value(QStringLiteral("pos")).toMap().value(QStringLiteral("den")).toInt(), 3);
    // delta 带小节位移
    const auto r4 = u.addPosDelta(makeRef(1, 0, 1, 1, "key", 0, 1), makeDelta(2, 0, 1));
    EXPECT_EQ(r4.value(QStringLiteral("measure")).toInt(), 3);
    // den = 0 → 空 map（不崩）
    EXPECT_TRUE(u.addPosDelta(makeRef(0, 1, 4, 1, "key", 0, 1), makeDelta(0, 1, 0)).isEmpty());
}

TEST(EditUtils, LooksLikeBmsText) {
    EditUtils u;
    EXPECT_TRUE(u.looksLikeBmsText(QStringLiteral("#00101:0100")));
    EXPECT_TRUE(u.looksLikeBmsText(QStringLiteral("#WAV01 kick.wav")));
    EXPECT_TRUE(u.looksLikeBmsText(QStringLiteral("  #00102:0.75")));
    EXPECT_TRUE(u.looksLikeBmsText(QStringLiteral("#TITLE x\n#00101:AA")));
    EXPECT_FALSE(u.looksLikeBmsText(QStringLiteral("#TITLE My Song")));
    EXPECT_FALSE(u.looksLikeBmsText(QStringLiteral("hello world")));
    EXPECT_FALSE(u.looksLikeBmsText(QString()));
    EXPECT_FALSE(u.looksLikeBmsText(QStringLiteral("; #00101:AA")));  // 注释行
}

TEST(EditUtils, TextUsesBase62Ids) {
    EditUtils u;
    // #BASE 62 声明（Base62 片段标记）
    EXPECT_TRUE(u.textUsesBase62Ids(QStringLiteral("#BASE 62\n#WAVzz slice_000.wav")));
    EXPECT_TRUE(u.textUsesBase62Ids(QStringLiteral("#base 62")));
    // 小写 id（> 1295 的 Base62 值）
    EXPECT_TRUE(u.textUsesBase62Ids(QStringLiteral("#WAVzz slice_000.wav")));
    EXPECT_TRUE(u.textUsesBase62Ids(QStringLiteral("#00101:zz")));
    // Base36 常态：大写/数字 id、文件名小写不误报
    EXPECT_FALSE(u.textUsesBase62Ids(QStringLiteral("#WAV01 kick.wav")));
    EXPECT_FALSE(u.textUsesBase62Ids(QStringLiteral("#00101:AA00ZZ")));
    EXPECT_FALSE(u.textUsesBase62Ids(QStringLiteral("#00102:0.75")));
    EXPECT_FALSE(u.textUsesBase62Ids(QStringLiteral("#TITLE lowercase title")));
    EXPECT_FALSE(u.textUsesBase62Ids(QString()));
}
