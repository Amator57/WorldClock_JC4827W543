#pragma once

#include <stdint.h>
#include <stddef.h>

//============================================================
// Pre-generated PCM phrase sequence (Andriy Tkachenko):
//   "Ура!"  [1s pause]  "Кінець робочого тижня!"  [1s pause]  "Вихідні!"
//
// Mono, 22050 Hz, 16-bit signed.
//============================================================

extern const int16_t *PHRASE_URA;
extern const size_t   PHRASE_URA_SAMPLES;

// Generate the placeholder data. Called once from audioInit().
void phraseInit();

