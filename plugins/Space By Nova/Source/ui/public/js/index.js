let Juce = null;

const parameterStates = {};
const modeNames = ["Studio", "Arena", "Dream", "Vintage"];
const macroParams = ["space", "air", "depth", "mix", "width"];

const presets = {
  Studio:  { space: 18, air: 32, depth: 26, mix: 16, width: 38 },
  Arena:   { space: 66, air: 60, depth: 70, mix: 28, width: 86 },
  Dream:   { space: 84, air: 64, depth: 82, mix: 34, width: 90 },
  Vintage: { space: 42, air: 20, depth: 52, mix: 24, width: 48 }
};

const currentValues = { ...presets.Studio };
let juceAvailable = false;
let activeDrag = null;

function clamp(value, min = 0, max = 100) {
  return Math.min(max, Math.max(min, value));
}

function moveCaretToEnd(element) {
  const selection = window.getSelection();
  const range = document.createRange();
  range.selectNodeContents(element);
  range.collapse(false);
  selection.removeAllRanges();
  selection.addRange(range);
}

function updateVisual(param, percent) {
  const safeValue = clamp(Number(percent) || 0);
  const knob = document.getElementById(`${param}-knob`);
  const readout = document.getElementById(`${param}-value`);

  currentValues[param] = safeValue;

  if (knob) {
    const indicator = knob.querySelector(".indicator");
    const rotation = -120 + (safeValue / 100) * 240;
    indicator?.style.setProperty("--rotation", `${rotation}deg`);
  }

  if (readout && document.activeElement !== readout) {
    readout.textContent = `${Math.round(safeValue)}%`;
  }
}

function pushToBackend(param, percent, useGesture = false) {
  const state = parameterStates[param];
  if (!state) return;

  try {
    if (useGesture) state.sliderDragStarted();
    state.setNormalisedValue(clamp(percent) / 100);
    if (useGesture) state.sliderDragEnded();
  } catch (error) {
    console.warn(`Failed to update ${param}:`, error.message);
  }
}

function setParamPercent(param, percent, options = {}) {
  const safeValue = clamp(percent);
  updateVisual(param, safeValue);

  if (options.pushToBackend) {
    pushToBackend(param, safeValue, !!options.useGesture);
  }
}

function setActiveMode(modeName) {
  document.querySelectorAll(".mode-button").forEach((button) => {
    button.classList.toggle("active", button.dataset.mode === modeName);
  });
}

function applyPreset(modeName, pushToBackend = true) {
  const preset = presets[modeName];
  if (!preset) return;

  setActiveMode(modeName);

  macroParams.forEach((param) => {
    setParamPercent(param, preset[param], { pushToBackend });
  });

  if (pushToBackend && parameterStates.nova_mode) {
    try {
      const modeIndex = modeNames.indexOf(modeName);
      parameterStates.nova_mode.setNormalisedValue(modeIndex / (modeNames.length - 1));
    } catch (error) {
      console.warn("Failed to set mode:", error.message);
    }
  }
}

function initialiseParameterStates() {
  if (!juceAvailable || !Juce) return;

  [...macroParams, "nova_mode"].forEach((param) => {
    try {
      parameterStates[param] = Juce.getSliderState(param);
    } catch (error) {
      console.warn(`Could not create slider state for ${param}:`, error.message);
    }
  });

  macroParams.forEach((param) => {
    const state = parameterStates[param];
    if (!state?.valueChangedEvent) return;

    state.valueChangedEvent.addListener(() => {
      updateVisual(param, state.getNormalisedValue() * 100);
    });
  });

  if (parameterStates.nova_mode?.valueChangedEvent) {
    parameterStates.nova_mode.valueChangedEvent.addListener(() => {
      const modeIndex = Math.round(parameterStates.nova_mode.getNormalisedValue() * (modeNames.length - 1));
      setActiveMode(modeNames[clamp(modeIndex, 0, modeNames.length - 1)]);
    });
  }

  window.setTimeout(() => {
    macroParams.forEach((param) => {
      const state = parameterStates[param];
      if (state) updateVisual(param, state.getNormalisedValue() * 100);
    });

    if (parameterStates.nova_mode) {
      const modeIndex = Math.round(parameterStates.nova_mode.getNormalisedValue() * (modeNames.length - 1));
      setActiveMode(modeNames[clamp(modeIndex, 0, modeNames.length - 1)]);
    }
  }, 50);
}

function finishActiveDrag(pointerId = null) {
  if (!activeDrag) return;

  if (pointerId !== null && activeDrag.pointerId !== null && pointerId !== activeDrag.pointerId) {
    return;
  }

  const state = parameterStates[activeDrag.param];
  if (state) {
    try { state.sliderDragEnded(); } catch (error) { console.warn(error.message); }
  }

  if (typeof activeDrag.knob.releasePointerCapture === "function" && activeDrag.pointerId !== null) {
    try { activeDrag.knob.releasePointerCapture(activeDrag.pointerId); } catch (error) { console.warn(error.message); }
  }

  activeDrag.knob.classList.remove("dragging");
  activeDrag = null;
}

function initialiseKnobs() {
  document.querySelectorAll(".macro-knob").forEach((knob) => {
    const param = knob.dataset.param;
    const defaultPercent = Number(knob.dataset.default || 0);

    updateVisual(param, currentValues[param] ?? defaultPercent);

    knob.addEventListener("pointerdown", (event) => {
      if (event.button !== undefined && event.button !== 0) return;

      event.preventDefault();
      event.stopPropagation();

      knob.classList.add("dragging");
      activeDrag = {
        param,
        knob,
        pointerId: event.pointerId ?? null,
        startY: event.clientY,
        startValue: currentValues[param] ?? defaultPercent
      };

      if (typeof knob.setPointerCapture === "function" && event.pointerId !== undefined) {
        try { knob.setPointerCapture(event.pointerId); } catch (error) { console.warn(error.message); }
      }

      const state = parameterStates[param];
      if (state) {
        try { state.sliderDragStarted(); } catch (error) { console.warn(error.message); }
      }
    });

    knob.addEventListener("dblclick", () => {
      setParamPercent(param, defaultPercent, { pushToBackend: true });
    });
  });

  document.addEventListener("pointermove", (event) => {
    if (!activeDrag) return;
    if (activeDrag.pointerId !== null && event.pointerId !== activeDrag.pointerId) return;

    event.preventDefault();

    const deltaY = activeDrag.startY - event.clientY;
    const nextValue = clamp(activeDrag.startValue + (deltaY * 0.45));
    setParamPercent(activeDrag.param, nextValue, { pushToBackend: true });
  });

  document.addEventListener("pointerup", (event) => {
    finishActiveDrag(event.pointerId ?? null);
  });

  document.addEventListener("pointercancel", (event) => {
    finishActiveDrag(event.pointerId ?? null);
  });
}

function initialiseReadouts() {
  document.querySelectorAll(".editable-value").forEach((readout) => {
    const { param } = readout.dataset;

    readout.addEventListener("focus", () => {
      readout.textContent = `${Math.round(currentValues[param] ?? 0)}`;
      moveCaretToEnd(readout);
    });

    readout.addEventListener("input", () => {
      const cleaned = readout.textContent.replace(/[^0-9.]/g, "");
      if (readout.textContent !== cleaned) {
        readout.textContent = cleaned;
        moveCaretToEnd(readout);
      }
    });

    readout.addEventListener("keydown", (event) => {
      if (event.key === "Enter") {
        event.preventDefault();
        readout.blur();
      }

      if (event.key === "Escape") {
        event.preventDefault();
        updateVisual(param, currentValues[param] ?? 0);
        readout.blur();
      }
    });

    readout.addEventListener("blur", () => {
      const raw = readout.textContent.replace(/[^0-9.]/g, "");
      const nextValue = raw === "" ? (currentValues[param] ?? 0) : clamp(Number(raw));
      setParamPercent(param, nextValue, { pushToBackend: true });
    });
  });
}

function initialiseModeButtons() {
  document.querySelectorAll(".mode-button").forEach((button) => {
    button.addEventListener("click", () => {
      applyPreset(button.dataset.mode, true);
    });
  });
}

async function bootUi() {
  juceAvailable = typeof window.__JUCE__ !== "undefined";

  if (juceAvailable) {
    try {
      Juce = await import("./juce/index.js");
    } catch (error) {
      juceAvailable = false;
      console.warn("JUCE bridge unavailable, falling back to preview mode:", error.message);
    }
  }

  initialiseKnobs();
  initialiseReadouts();
  initialiseModeButtons();

  if (juceAvailable) {
    try {
      initialiseParameterStates();
    } catch (error) {
      juceAvailable = false;
      console.warn("Parameter sync unavailable, keeping local UI active:", error.message);
      applyPreset("Studio", false);
    }
  } else {
    applyPreset("Studio", false);
  }
}

if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", () => { void bootUi(); }, { once: true });
} else {
  void bootUi();
}
