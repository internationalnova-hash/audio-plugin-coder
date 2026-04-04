import * as Juce from "./juce/index.js";

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
      parameterStates.nova_mode.setNormalisedValue(modeNames.indexOf(modeName) / 3);
    } catch (error) {
      console.warn("Failed to set mode:", error.message);
    }
  }
}

function initialiseParameterStates() {
  if (!juceAvailable) return;

  [...macroParams, "nova_mode"].forEach((param) => {
    try {
      parameterStates[param] = Juce.getSliderState(param);
    } catch (error) {
      console.warn(`Could not create slider state for ${param}:`, error.message);
    }
  });

  macroParams.forEach((param) => {
    const state = parameterStates[param];
    if (!state) return;

    state.addValueChangedListener(() => {
      updateVisual(param, state.getNormalisedValue() * 100);
    });
  });

  if (parameterStates.nova_mode) {
    parameterStates.nova_mode.addValueChangedListener(() => {
      const modeIndex = Math.round(parameterStates.nova_mode.getNormalisedValue() * 3);
      setActiveMode(modeNames[clamp(modeIndex, 0, 3)]);
    });
  }

  window.setTimeout(() => {
    macroParams.forEach((param) => {
      const state = parameterStates[param];
      if (state) updateVisual(param, state.getNormalisedValue() * 100);
    });

    if (parameterStates.nova_mode) {
      const modeIndex = Math.round(parameterStates.nova_mode.getNormalisedValue() * 3);
      setActiveMode(modeNames[clamp(modeIndex, 0, 3)]);
    }
  }, 50);
}

function initialiseKnobs() {
  document.querySelectorAll(".macro-knob").forEach((knob) => {
    const param = knob.dataset.param;
    const defaultPercent = Number(knob.dataset.default || 0);

    updateVisual(param, currentValues[param] ?? defaultPercent);

    knob.addEventListener("mousedown", (event) => {
      event.preventDefault();
      knob.classList.add("dragging");
      activeDrag = {
        param,
        knob,
        startY: event.clientY,
        startValue: currentValues[param] ?? defaultPercent
      };

      const state = parameterStates[param];
      if (state) {
        try { state.sliderDragStarted(); } catch (error) { console.warn(error.message); }
      }
    });

    knob.addEventListener("dblclick", () => {
      setParamPercent(param, defaultPercent, { pushToBackend: true });
    });
  });

  document.addEventListener("mousemove", (event) => {
    if (!activeDrag) return;

    const deltaY = activeDrag.startY - event.clientY;
    const nextValue = clamp(activeDrag.startValue + deltaY * 0.45);
    setParamPercent(activeDrag.param, nextValue, { pushToBackend: true });
  });

  document.addEventListener("mouseup", () => {
    if (!activeDrag) return;

    const state = parameterStates[activeDrag.param];
    if (state) {
      try { state.sliderDragEnded(); } catch (error) { console.warn(error.message); }
    }

    activeDrag.knob.classList.remove("dragging");
    activeDrag = null;
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

document.addEventListener("DOMContentLoaded", () => {
  juceAvailable = typeof window.__JUCE__ !== "undefined";
  initialiseParameterStates();
  initialiseKnobs();
  initialiseReadouts();
  initialiseModeButtons();

  if (!juceAvailable) {
    applyPreset("Studio", false);
  }
});
