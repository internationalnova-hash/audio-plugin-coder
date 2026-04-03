import * as Juce from "./juce/index.js";

const modeCopy = {
    0: ["Studio glow for modern vocals", "Tight, polished, lead-vocal ambience."],
    1: ["Wide modern lift with bigger walls", "Open, glossy, performance-sized space."],
    2: ["Soft bloom with long airy motion", "Diffuse ambient trail for cinematic vocals."],
    3: ["Warm plate memory with golden decay", "Darker, richer, hardware-inspired tail."]
};

const sliderConfig = {
    space: { defaultValue: 0.45, decimals: 0 },
    air: { defaultValue: 0.60, decimals: 0 },
    depth: { defaultValue: 0.40, decimals: 0 },
    mix: { defaultValue: 0.18, decimals: 0 },
    width: { defaultValue: 0.80, decimals: 0 }
};

const sliderStates = new Map();
const knobControllers = new Map();
let modeState = null;

function getSliderState(name) {
    try {
        return Juce.getSliderState(name);
    } catch {
        let current = sliderConfig[name]?.defaultValue ?? 0.5;
        return {
            getNormalisedValue: () => current,
            setNormalisedValue: (value) => { current = Math.max(0, Math.min(1, value)); },
            sliderDragStarted: () => {},
            sliderDragEnded: () => {},
            valueChangedEvent: { addListener: () => {} }
        };
    }
}

function createModeState() {
    try {
        return Juce.getSliderState("nova_mode");
    } catch {
        let current = 0;
        return {
            getNormalisedValue: () => current / 3,
            setNormalisedValue: (value) => { current = Math.round(Math.max(0, Math.min(1, value)) * 3); },
            sliderDragStarted: () => {},
            sliderDragEnded: () => {},
            valueChangedEvent: { addListener: () => {} }
        };
    }
}

function normalisedToRotation(value) {
    return -120 + value * 240;
}

function formatPercent(value, decimals = 0) {
    return `${(value * 100).toFixed(decimals)}%`;
}

function updateModeCopy(index) {
    const [subtitle, description] = modeCopy[index] ?? modeCopy[0];
    document.getElementById("modeSubtitle").textContent = subtitle;
    document.getElementById("modeDescription").textContent = description;

    document.querySelectorAll(".mode").forEach((button) => {
        button.classList.toggle("active", Number(button.dataset.modeIndex) === index);
    });
}

function createKnobController(paramName) {
    const state = getSliderState(paramName);
    const knob = document.getElementById(`${paramName}-knob`);
    const valueNode = document.getElementById(`${paramName}-value`);
    const { defaultValue, decimals } = sliderConfig[paramName];

    const setVisual = (normalised) => {
        knob.style.setProperty("--rotation", `${normalisedToRotation(normalised)}deg`);
        valueNode.textContent = formatPercent(normalised, decimals);
    };

    const setFromUser = (normalised) => {
        const clamped = Math.max(0, Math.min(1, normalised));
        setVisual(clamped);
        state.setNormalisedValue(clamped);
    };

    let dragging = false;
    let lastY = 0;

    const startDrag = (event) => {
        if (dragging) {
            return;
        }

        dragging = true;
        lastY = event.clientY;

        if (event.pointerId !== undefined && knob.setPointerCapture) {
            knob.setPointerCapture(event.pointerId);
        }

        state.sliderDragStarted();
        event.preventDefault();
        event.stopPropagation();
    };

    const moveDrag = (event) => {
        if (!dragging) {
            return;
        }

        const deltaY = lastY - event.clientY;
        lastY = event.clientY;
        setFromUser(state.getNormalisedValue() + deltaY * 0.005);
        event.preventDefault();
    };

    const finishDrag = (event) => {
        if (!dragging) {
            return;
        }

        dragging = false;
        state.sliderDragEnded();

        if (event?.pointerId !== undefined && knob.releasePointerCapture) {
            try {
                knob.releasePointerCapture(event.pointerId);
            } catch {
                // Ignore missing capture state.
            }
        }
    };

    knob.addEventListener("pointerdown", startDrag);
    document.addEventListener("pointermove", moveDrag, true);
    document.addEventListener("pointerup", finishDrag, true);
    document.addEventListener("pointercancel", finishDrag, true);

    knob.addEventListener("mousedown", startDrag);
    document.addEventListener("mousemove", moveDrag, true);
    document.addEventListener("mouseup", finishDrag, true);

    knob.addEventListener("wheel", (event) => {
        event.preventDefault();
        event.stopPropagation();
        const step = event.deltaY > 0 ? -0.02 : 0.02;
        setFromUser(state.getNormalisedValue() + step);
    }, { passive: false });

    knob.addEventListener("dblclick", () => setFromUser(defaultValue));

    state.valueChangedEvent.addListener(() => setVisual(state.getNormalisedValue()));
    setVisual(state.getNormalisedValue());

    return { setVisual };
}

function initializeModeButtons() {
    modeState = createModeState();
    const updateFromState = () => {
        const index = Math.round(modeState.getNormalisedValue() * 3);
        updateModeCopy(index);
    };

    document.querySelectorAll(".mode").forEach((button) => {
        button.addEventListener("click", () => {
            const index = Number(button.dataset.modeIndex);
            modeState.sliderDragStarted();
            modeState.setNormalisedValue(index / 3);
            modeState.sliderDragEnded();
            updateModeCopy(index);
        });
    });

    modeState.valueChangedEvent.addListener(updateFromState);
    updateFromState();
}

document.addEventListener("DOMContentLoaded", () => {
    Object.keys(sliderConfig).forEach((name) => {
        const state = getSliderState(name);
        sliderStates.set(name, state);
        knobControllers.set(name, createKnobController(name));
    });

    initializeModeButtons();
});