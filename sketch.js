/*
Yaad UI
*/

//Global Variables
let BYPASS = 0;
let TEMPO = 60;
let FDBK = 0;
let DRY_WET = 0.5;
let SPREAD = 0;
let DETUNE = 0;
let N_TAPS = 1;
let maxTaps = 5;
let INPUT_LVL = 0.5;
let OUTPUT_LVL = 0.5;

//GLOBAL DATA
const noteVals = {
  0: 0.0,
  "1/128": 0.03125,
  "1/64": 0.0625,
  "1/32T": 0.08333,
  "1/64D": 0.09375,
  "1/32": 0.125,
  "1/16T": 0.1667,
  "1/32D": 0.1875,
  "1/16": 0.25,
  "1/8T": 0.3333,
  "1/16D": 0.375,
  "1/8": 0.5,
  "1/4T": 0.6667,
  "1/8D": 0.75,
  "1/4": 1,
  "1/2T": 1.3333,
  "1/4D": 1.5,
  "1/2": 2,
  "1/1T": 2.6667,
  "1/2D": 3,
  "1/1": 4,
  "1/1D": 6,
};
const semiTones = Array.from({ length: 49 }, (v, i) => i - 24);
const dirVals = {
  Fwd: [1, 0],
  Rev: [-1, 180],
  Rnd: [0, -90],
};
const envVals = {
  Bell: [0, 0],
  Perc: [1, 30],
  Swel: [-1, -30],
};

// Arrays to be sent to Bela
let globalParams = new Array(65);

//OBJECTS
let bypassButton;
let tempoBox;
let fdbkKnob;
let dryWetKnob;
let spreadKnob;
let detuneKnob;
let nTapsKnob;
let inKnob;
let outKnob;
let tapSels = [];
let tapCtrls = [];
let xy_axis;
let myFont;

// Position Vars
let nKnobs = 8;
let s;
let kD;
let tD;
let x;
let y;
let lx;
let ly;

// Data Buffers

function preload() {
  myFont = loadFont("/projects/yaad_0.3/assets/NunitoSans-Light.ttf");
}

function setup() {
  // Set GUI positions wrt to window width and height
  s = windowWidth / (5 * nKnobs + 1);
  kD = 4 * s;
  tD = 3 * s;
  sD = s;
  x = 3 * s;
  y = 3 * s;
  lx = (nKnobs - 1) * kD + (nKnobs - 6) * s;
  ly = nKnobs * sD + (nKnobs - 4) * s;

  createCanvas(windowWidth, windowHeight);
  angleMode(RADIANS);

  // GUI ELEMENTS
  bypassButton = new button(x / 4, y / 4, kD / 4);
  tapTempo = new tapButton(x, kD + 1.5 * s, kD / 4);

  let dispX = x;
  let incrX = kD + s;
  tempoBox = new textBox(dispX, y, kD, 1, 300, 60);
  dispX += incrX;
  fdbkKnob = new knob(dispX, y, kD, 0, 1, FDBK, 100, "Feedback");
  dispX += incrX;
  dryWetKnob = new knob(dispX, y, kD, 0, 1, DRY_WET, 100, "Dry/Wet");
  dispX += incrX;
  spreadKnob = new knob(dispX, y, kD, 0, 1, SPREAD, 100, "Spread");
  dispX += incrX;
  detuneKnob = new knob(dispX, y, kD, 0, 1, DETUNE, 100, "Detune");
  dispX += incrX;
  nTapsKnob = new knob(dispX, y, kD, 1, maxTaps, N_TAPS, maxTaps-1, "Taps");
  dispX += incrX;
  inKnob = new knob(dispX, y, kD, 0, 2, INPUT_LVL, 100, "In");
  dispX += incrX;
  outKnob = new knob(dispX, y, kD, 0, 2, OUTPUT_LVL, 100, "Out");

  let y_Ax = kD + 4 * sD + 6 * s;
  xy_axis = new axes(x, y_Ax, lx, ly);

  let dispY = y_Ax - ly / 2 + sD / 2;

  for (let i = 0; i < maxTaps; i++) {
    let incrY = sD + (ly - sD * nKnobs) / (nKnobs - 1);
    tapSels.push(new tapSelector(dispX, dispY, sD, i + 1));
    tapCtrls.push(new tapMainCtrl(x, y_Ax, tD, tapSels[i], i + 1));
    dispY += incrY;
  }
}

function draw() {
  background("#E91E63");

  bypassButton.display();
  globalParams[0] = BYPASS = bypassButton.currVal;

  tempoBox.display();
  globalParams[1] = TEMPO = tempoBox.currVal;

  tapTempo.display();

  fdbkKnob.display("LABEL");
  globalParams[2] = FDBK = fdbkKnob.currVal;

  dryWetKnob.display("LABEL");
  globalParams[3] = DRY_WET = dryWetKnob.currVal;

  spreadKnob.display("LABEL");
  globalParams[4] = SPREAD = spreadKnob.currVal;

  detuneKnob.display("LABEL");
  globalParams[5] = DETUNE = detuneKnob.currVal;

  nTapsKnob.display("LABEL");
  globalParams[6] = N_TAPS = nTapsKnob.currVal;

  inKnob.display("LABEL");
  globalParams[7] = INPUT_LVL = inKnob.currVal;

  outKnob.display("LABEL");
  globalParams[8] = OUTPUT_LVL = outKnob.currVal;

  xy_axis.display(5, Object.values(noteVals), semiTones);

  for (let i = 0; i < N_TAPS; i++) {
    tapSels[i].display();
    tapCtrls[i].display();
  }
  
  for (let i = 0; i < maxTaps; i++) {
    globalParams[7*i+9]  = (60000 * noteVals[tapCtrls[i].delVal]) / TEMPO;
    globalParams[7*i+10] = (60000 * noteVals[tapCtrls[i].szVal]) / TEMPO;
    globalParams[7*i+11] = tapCtrls[i].rateVal;
    globalParams[7*i+12] = dirVals[tapCtrls[i].dirVal][0];
    globalParams[7*i+13] = envVals[tapCtrls[i].envVal][0];
    globalParams[7*i+14] = tapCtrls[i].lpfVal;
    globalParams[7*i+15] = tapCtrls[i].hpfVal;
  }
  
  // Send data to Bela 	
  Bela.data.sendBuffer(0, 'float', globalParams);
  
}

function mouseClicked() {
  /*
  bypassButton can be toggled on/off on click
  tapTempo can be tapped on to set tempo
  */
  bypassButton.mouseClick();
  tapTempo.mouseClick(millis(), tempoBox);
  for (let i = 0; i < N_TAPS; i++) {
    tapSels[i].mouseClick();
  }
}

function mouseDragged() {
  /*
  Knobs can be dragged on (Y-pos only) to set values.
  tapCtrls can be dragged on:
    X-pos -> Delay Time
    Y-pos -> Rate in semitones
    drag on sz -> Change grain size
    drag on LPF -> Change LPF cutoff
    drag on HPF -> Change HPF cutoff
  */
  tempoBox.mouseDrag();
  fdbkKnob.mouseDrag();
  dryWetKnob.mouseDrag();
  spreadKnob.mouseDrag();
  detuneKnob.mouseDrag();
  nTapsKnob.mouseDrag();
  inKnob.mouseDrag();
  outKnob.mouseDrag();

  for (let i = 0; i < N_TAPS; i++) {
    tapCtrls[i].moveX(xy_axis.lx, noteVals);
    tapCtrls[i].moveY(xy_axis.ly, semiTones);
    tapCtrls[i].changeSz(noteVals);
    //tapCtrls[i].changeLPF();
    //tapCtrls[i].changeHPF();
  }

  return false;
}

function doubleClicked() {
  /*
  some tapCtrls can be double clicked:
    double click on env -> Change grain envelope (Bell, Perc, Swel)
    double click on dir -> Change playback direction (Fwd, Rev, Rnd)
  */
  for (let i = 0; i < N_TAPS; i++) {
    tapCtrls[i].changeEnv(envVals);
    tapCtrls[i].changeDir(dirVals);
  }
}

function mouseReleased() {
  exitPointerLock();
}

class UI_Factory {
  constructor(x_pos, y_pos, diameter) {
    this.x_pos = x_pos;
    this.y_pos = y_pos;
    this.diameter = diameter;
    this.mousePos = 0;
    this.mouseOn = false;
    this.isPressed = false;
    this.currVal = 0;
    this.onColor = "#1ee9a4";
    this.offColor = "#E91E63";
    this.currColor = "#E91E63";
    this.font = myFont;
  }

  calcMousePos() {
    this.mousePos = dist(this.x_pos, this.y_pos, mouseX, mouseY);
  }

  mouseHover() {
    this.mouseOn = this.mousePos > 0 && this.mousePos <= this.diameter / 2;
  }

  mousePressed() {
    if (mouseIsPressed && this.mouseOn) {
      this.isPressed = true;
    } else if (!mouseIsPressed) {
      this.isPressed = false;
    }
  }

  display() {
    this.calcMousePos();
    this.mouseHover();
    this.mousePressed();
    push();
    fill("#FFFFFF");
    noStroke();
    circle(this.x_pos, this.y_pos, this.diameter);
    pop();
    this.currColor = this.mouseOn ? this.onColor : this.offColor;
  }
}

class button extends UI_Factory {
  constructor(x_pos, y_pos, diameter) {
    super(x_pos, y_pos, diameter);
  }

  mouseClick() {
    if (this.mouseOn) {
      this.currColor =
        this.currColor == this.offColor ? this.onColor : this.offColor;
      this.currVal = this.currVal == 0 ? 1 : 0;
    }
  }

  display() {
    this.calcMousePos();
    this.mouseHover();
    this.mousePressed();
    push();
    fill("#FFFFFF");
    noStroke();
    circle(this.x_pos, this.y_pos, this.diameter);
    pop();

    push();
    stroke(this.currColor);
    strokeWeight(4);
    line(
      this.x_pos,
      this.y_pos - this.diameter / 4,
      this.x_pos,
      this.y_pos + this.diameter / 4
    );
    pop();
  }
}

class tapButton extends UI_Factory {
  constructor(x_pos, y_pos, diameter) {
    super(x_pos, y_pos, diameter);
    this.prevTime = 0;
    this.interval = 0;
    this.sum = 0;
    this.idx = 0;
  }

  mouseClick(currTime, objToUpdate) {
    if (this.mouseOn) {
      this.interval = currTime - this.prevTime;
      this.prevTime = currTime;
      this.sum += 60000 / this.interval;
      let avg = int(this.sum / (this.idx + 1));
      if (++this.idx >= 4) {
        this.idx = 0;
        this.sum = 0;
      }
      objToUpdate.currVal = avg;
    }
  }

  display() {
    super.display();
    push();
    this.currColor =
      mouseIsPressed && this.mouseOn ? this.onColor : this.offColor;
    stroke(this.currColor);
    strokeWeight(2);
    line(
      this.x_pos - this.diameter / 4,
      this.y_pos - this.diameter / 8,
      this.x_pos + this.diameter / 4,
      this.y_pos - this.diameter / 8
    );
    line(
      this.x_pos,
      this.y_pos - this.diameter / 4,
      this.x_pos,
      this.y_pos + this.diameter / 4
    );
    pop();
  }
}

class textBox extends UI_Factory {
  constructor(x_pos, y_pos, diameter, valLo, valHi, initVal) {
    super(x_pos, y_pos, diameter);
    super.currVal = initVal;
    this.valLo = valLo;
    this.valHi = valHi;
  }

  mouseDrag() {
    if (this.mouseOn) {
      requestPointerLock();
      let incr = -movedY;
      this.currVal += incr;
      this.currVal = constrain(this.currVal, this.valLo, this.valHi);
    }
  }

  display() {
    super.display();

    push();
    fill("#FFFFFF");
    textFont(this.font);
    textSize(this.diameter / 6);
    textAlign(CENTER, TOP);
    text("Tempo", this.x_pos, this.y_pos - 0.75 * this.diameter);
    pop();

    push();
    this.currColor = this.mouseOn ? this.onColor : this.offColor;
    textFont(this.font);
    textSize(this.diameter / 2);
    textAlign(CENTER, TOP);
    fill(this.currColor);
    let ofst = this.diameter / 3;
    text(this.currVal, this.x_pos, this.y_pos - ofst * 1.25);
    textFont(this.font);
    textSize(this.diameter / 4);
    textAlign(CENTER, TOP);
    fill(this.currColor);
    text("BPM", this.x_pos, this.y_pos + ofst / 6);
    pop();
  }
}

class knob extends UI_Factory {
  constructor(x_pos, y_pos, diameter, valLo, valHi, initVal, numVals, label) {
    super(x_pos, y_pos, diameter);
    this.currVal = initVal;
    this.valLo = valLo;
    this.valHi = valHi;
    this.numVals = numVals;
    this.angleLo = -PI / 3;
    this.angleHi = (4 * PI) / 3;
    this.currAngle =
      this.angleLo +
      ((this.currVal - this.valLo) * (this.angleHi - this.angleLo)) /
        (this.valHi - this.valLo);
    this.label = label;
  }

  mouseDrag() {
    if (this.mouseOn) {
      requestPointerLock();
      let angleIncr = (this.angleHi - this.angleLo) / this.numVals;
      let valIncr = (this.valHi - this.valLo) / this.numVals;
      this.currAngle += angleIncr * -movedY;
      this.currVal += valIncr * -movedY;
      this.currAngle = constrain(this.currAngle, this.angleLo, this.angleHi);
      this.currVal = constrain(round(this.currVal, 3), this.valLo, this.valHi);
    }
  }

  display(labelState) {
    super.display();

    push();
    translate(this.x_pos, this.y_pos);
    rotate(this.currAngle);
    this.currColor = this.mouseOn ? this.onColor : this.offColor;
    stroke(this.currColor);
    strokeWeight(this.diameter / 8);
    this.mouseOn
      ? line(-this.diameter / 16, 0, -this.diameter / 2, 0)
      : line(-this.diameter / 8, 0, -this.diameter / 3, 0);
    pop();

    if (labelState == "LABEL") {
      push();
      fill("#FFFFFF");
      textFont(this.font);
      textSize(this.diameter / 6);
      textAlign(CENTER, TOP);
      text(this.label, this.x_pos, this.y_pos - 0.75 * this.diameter);
      pop();

      push();
      fill("#FFFFFF");
      textFont(this.font);
      textSize(this.diameter / 6);
      textAlign(CENTER, BOTTOM);
      text(this.currVal, this.x_pos, this.y_pos + 0.75 * this.diameter);
      pop();
    } else if (labelState == "NO_LABEL") {
    }
  }
}

class tapSelector extends UI_Factory {
  constructor(x_pos, y_pos, diameter, label) {
    super(x_pos, y_pos, diameter);
    this.label = label;
    this.isActive = false;
  }

  mouseClick() {
    if (this.mouseOn) {
      this.isActive = this.isActive == false ? true : false;
    }
  }

  display() {
    super.display();

    push();
    textFont(this.font);
    textSize(this.diameter);
    textAlign(CENTER, BASELINE);
    this.currColor = this.isActive ? this.onColor : this.offColor;
    fill(this.currColor);
    text(this.label, this.x_pos, this.y_pos + this.diameter / 3);
    pop();
  }
}

class tapMainCtrl extends UI_Factory {
  constructor(x_pos, y_pos, diameter, selector, label) {
    super(x_pos, y_pos, diameter);
    this.currPosX = x_pos;
    this.currPosY = y_pos;
    this.currDiameter = map(14 / 22, 0, 1, 1 / 2, 1, true) * this.diameter;
    this.label = label;
    this.selector = selector;

    this.szCtrl = new UI_Factory(this.currPosX, this.currPosY, this.diameter);
    this.dirCtrl = new UI_Factory(this.currPosX, this.currPosY, this.diameter);
    this.envCtrl = new UI_Factory(this.currPosX, this.currPosY, this.diameter);
    this.lpfCtrl = new knob(
      this.currPosX,
      this.currPosY,
      this.diameter,
      20,
      20000,
      20,
      1000,
      "LPF"
    );

    this.hpfCtrl = new knob(
      this.currPosX,
      this.currPosY,
      this.diameter,
      20,
      20000,
      20000,
      1000,
      "HPF"
    );

    this.delVal = "0";
    this.delIdx = 0;

    this.rateVal = 0;
    this.rateIdx = 25;

    this.szVal = "1/4";
    this.szIdx = 14;

    this.dirVal = "Fwd";
    this.dirIdx = 0;
    this.dirAngle = 0;

    this.envVal = "Bell";
    this.envIdx = 0;
    this.envShear = 0;

    this.lpfVal = this.lpfCtrl.currVal;
    this.hpfVal = this.hpfCtrl.currVal;
  }

  calcMousePos() {
    this.mousePos = dist(this.currPosX, this.currPosY, mouseX, mouseY);
  }

  mouseHover() {
    this.mouseOn = this.mousePos > 0 && this.mousePos <= this.diameter / 2;
  }

  moveX(rangeX, valX) {
    let v = Object.values(valX);
    let k = Object.keys(valX);
    if ((this.mouseOn || this.isPressed) && this.selector.isActive) {
      requestPointerLock();
      this.delIdx += movedX;
      this.delIdx = constrain(this.delIdx, 0, v.length - 1);
      let incr = this.delIdx / (v.length - 1);
      this.currPosX = this.x_pos + incr * rangeX;
      this.currPosX = constrain(this.currPosX, this.x_pos, this.x_pos + rangeX);
      this.delVal = k[this.delIdx];
    }
  }

  moveY(rangeY, valY) {
    let v = valY;
    if ((this.mouseOn || this.isPressed) && this.selector.isActive) {
      requestPointerLock();
      this.rateIdx += -movedY;
      this.rateIdx = constrain(this.rateIdx, 0, v.length - 1);
      let incr = v[this.rateIdx] / v[v.length - 1];
      this.currPosY = this.y_pos - incr * (rangeY / 2);
      this.rateVal = v[this.rateIdx];
    }
  }

  changeSz(valSz) {
    let v = Object.values(valSz);
    let k = Object.keys(valSz);
    if (
      (this.szCtrl.mouseOn || this.szCtrl.isPressed) &&
      this.selector.isActive
    ) {
      requestPointerLock();
      this.szIdx += -movedY;
      this.szIdx = constrain(this.szIdx, 0, v.length - 1);
      let incr = this.szIdx / (v.length - 1);
      incr = map(incr, 0, 1, 1 / 2, 1, true);
      this.currDiameter = this.diameter * incr;
      this.currDiameter = constrain(
        this.currDiameter,
        this.diameter / 4,
        this.diameter
      );
      this.szVal = k[this.szIdx];
    }
  }

  changeEnv(valEnv) {
    let v = Object.values(valEnv);
    let k = Object.keys(valEnv);
    if (
      (this.envCtrl.mouseOn || this.envCtrl.isPressed) &&
      this.selector.isActive
    ) {
      if (++this.envIdx >= 3) {
        this.envIdx = 0;
      }
      this.envVal = k[this.envIdx];
      this.envShear = radians(valEnv[this.envVal][1]);
    }
  }

  changeDir(valDir) {
    let v = Object.values(valDir);
    let k = Object.keys(valDir);
    if (
      (this.dirCtrl.mouseOn || this.dirCtrl.isPressed) &&
      this.selector.isActive
    ) {
      if (++this.dirIdx >= 3) {
        this.dirIdx = 0;
      }
      this.dirVal = k[this.dirIdx];
      this.dirAngle = radians(valDir[this.dirVal][1]);
    }
  }

  changeLPF() {
    this.lpfCtrl.mouseDrag();
    this.lpfVal = this.lpfCtrl.currVal;
  }

  changeHPF() {
    this.hpfCtrl.mouseDrag();
    this.hpfVal = this.hpfCtrl.currVal;
  }

  mainDisplay() {
    push();
    noStroke();
    circle(this.currPosX, this.currPosY, this.currDiameter);
    push();
    translate(this.currPosX, this.currPosY);
    textFont(this.font);
    textSize(this.diameter / 2);
    textAlign(CENTER, CENTER);
    fill(this.currColor);
    text(this.label, 0, 0 - this.diameter / 10);
    pop();
    pop();
  }

  szDisplay() {
    this.szCtrl.x_pos = this.currPosX + (this.diameter / 2 + 13) * cos(PI);
    this.szCtrl.y_pos = this.currPosY - (this.diameter / 2 + 13) * sin(PI);
    this.szCtrl.diameter = this.diameter / 4;

    this.szCtrl.display();

    push();
    translate(this.szCtrl.x_pos, this.szCtrl.y_pos);
    fill(this.szCtrl.currColor);
    noStroke();
    circle(0, -this.szCtrl.diameter / 6, this.szCtrl.diameter / 3);
    circle(0, this.szCtrl.diameter / 4.5, this.szCtrl.diameter / 6);

    if (this.szCtrl.mouseOn) {
      textFont(this.font);
      textSize(this.szCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text("Sz", 0, -this.szCtrl.diameter);
      text(this.szVal, 0, this.szCtrl.diameter / 1.125);
    }
    pop();
  }

  dirDisplay() {
    this.dirCtrl.x_pos = this.currPosX + (this.diameter / 2 + 13) * cos(PI / 3);
    this.dirCtrl.y_pos = this.currPosY - (this.diameter / 2 + 13) * sin(PI / 3);
    this.dirCtrl.diameter = this.diameter / 4;

    this.dirCtrl.display();

    push();
    translate(this.dirCtrl.x_pos, this.dirCtrl.y_pos);
    rotate(this.dirAngle);
    stroke(this.dirCtrl.currColor);
    strokeWeight(2);
    line(
      -this.dirCtrl.diameter / 8,
      this.dirCtrl.diameter / 4,
      this.dirCtrl.diameter / 4,
      0
    );
    line(
      -this.dirCtrl.diameter / 8,
      -this.dirCtrl.diameter / 4,
      this.dirCtrl.diameter / 4,
      0
    );
    pop();

    push();
    translate(this.dirCtrl.x_pos, this.dirCtrl.y_pos);
    noStroke();
    if (this.dirCtrl.mouseOn) {
      textFont(this.font);
      textSize(this.dirCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text(this.dirVal, 0, -this.dirCtrl.diameter);
    }
    pop();
  }

  envDisplay() {
    this.envCtrl.x_pos = this.currPosX + (this.diameter / 2 + 13) * cos(PI / 6);
    this.envCtrl.y_pos = this.currPosY - (this.diameter / 2 + 13) * sin(PI / 6);
    this.envCtrl.diameter = this.diameter / 4;

    this.envCtrl.display();

    push();
    translate(this.envCtrl.x_pos, this.envCtrl.y_pos);
    stroke(this.envCtrl.currColor);
    strokeWeight(2);
    shearX(this.envShear);
    line(
      this.envCtrl.diameter / 4,
      this.envCtrl.diameter / 8,
      0,
      -this.envCtrl.diameter / 4
    );
    line(
      -this.envCtrl.diameter / 4,
      this.envCtrl.diameter / 8,
      0,
      -this.envCtrl.diameter / 4
    );
    pop();

    push();
    translate(this.envCtrl.x_pos, this.envCtrl.y_pos);
    noStroke();
    if (this.envCtrl.mouseOn) {
      textFont(this.font);
      textSize(this.envCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text(this.envVal, 1.2 * this.envCtrl.diameter, 0);
    }
    pop();
  }

  lpfDisplay() {
    this.lpfCtrl.x_pos =
      this.currPosX + (this.diameter / 2 + 13) * cos(-PI / 3);
    this.lpfCtrl.y_pos =
      this.currPosY - (this.diameter / 2 + 13) * sin(-PI / 3);
    this.lpfCtrl.diameter = this.diameter / 4;

    this.lpfCtrl.display("NO_LABEL");

    push();
    translate(this.lpfCtrl.x_pos, this.lpfCtrl.y_pos);
    noStroke();
    if (this.lpfCtrl.mouseOn) {
      textFont(this.font);
      textSize(this.lpfCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text("LPF", 0, this.lpfCtrl.diameter / 1.125);
      text(this.lpfCtrl.currVal, 0, this.lpfCtrl.diameter * 1.5);
    }
    pop();
  }

  hpfDisplay() {
    this.hpfCtrl.x_pos =
      this.currPosX + (this.diameter / 2 + 13) * cos(-PI / 6);
    this.hpfCtrl.y_pos =
      this.currPosY - (this.diameter / 2 + 13) * sin(-PI / 6);
    this.hpfCtrl.diameter = this.diameter / 4;

    this.hpfCtrl.display("NO_LABEL");

    push();
    translate(this.hpfCtrl.x_pos, this.hpfCtrl.y_pos);
    noStroke();
    if (this.hpfCtrl.mouseOn) {
      textFont(this.font);
      textSize(this.hpfCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text("HPF", 1.25 * this.hpfCtrl.diameter, 0);
      text(
        this.hpfCtrl.currVal,
        1.25 * this.hpfCtrl.diameter,
        this.hpfCtrl.diameter * 0.625
      );
    }
    pop();
  }

  display() {
    this.calcMousePos();
    this.mouseHover();
    this.mousePressed();

    this.currColor = this.selector.isActive ? this.onColor : this.offColor;
    this.mainDisplay();
    if (this.selector.isActive) {
      this.szDisplay();
      this.dirDisplay();
      this.envDisplay();
      //this.lpfDisplay();
      //this.hpfDisplay();
    }

    if ((this.mouseOn || this.isPressed) && this.selector.isActive) {
      push();
      translate(
        this.currPosX + (this.diameter / 2 + 13) * cos((2 * PI) / 3),
        this.currPosY - (this.diameter / 2 + 13) * sin((2 * PI) / 3)
      );
      textFont(this.font);
      textSize(this.szCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text("D: " + this.delVal, 0, 0);
      pop();

      push();
      translate(
        this.currPosX + (this.diameter / 2 + 13) * cos((4 * PI) / 3),
        this.currPosY - (this.diameter / 2 + 13) * sin((4 * PI) / 3)
      );
      textFont(this.font);
      textSize(this.szCtrl.diameter / 2);
      textAlign(CENTER, CENTER);
      fill("#FFFFFF");
      text("R: " + this.rateVal, 0, 0);
      pop();
    }
  }
}

class axes {
  constructor(x_pos, y_pos, lx, ly) {
    this.x_pos = x_pos;
    this.y_pos = y_pos;
    this.lx = lx;
    this.ly = ly;
    this.font = myFont;
  }

  lineX() {
    push();
    stroke("#FFFFFF");
    strokeWeight(2);
    line(this.x_pos, this.y_pos, this.x_pos + this.lx, this.y_pos);
    pop();
  }

  lineY() {
    push();
    stroke("#FFFFFF");
    strokeWeight(2);
    line(
      this.x_pos,
      this.y_pos - this.ly / 2,
      this.x_pos,
      this.y_pos + this.ly / 2
    );
    pop();
  }

  markH(w, arr) {
    for (let i = 0; i < arr.length; i++) {
      let incr = i / (arr.length - 1);
      let x = this.x_pos + incr * this.lx;
      let y_1 = this.y_pos - w / 2;
      let y_2 = this.y_pos + w / 2;
      stroke("#FFFFFF");
      strokeWeight(2);
      line(x, y_1, x, y_2);
    }
  }

  markV(w, arr) {
    for (let i = 0; i < arr.length; i++) {
      let incr = arr[i] / arr[arr.length - 1];
      let y = this.y_pos + (incr * this.ly) / 2;
      let x_1 = this.x_pos - w / 2;
      let x_2 = this.x_pos + w / 2;
      stroke("#FFFFFF");
      strokeWeight(2);
      line(x_1, y, x_2, y);
    }
  }

  display(w, arrX, arrY) {
    push();
    this.lineX();
    this.lineY();
    this.markH(w, arrX);
    this.markV(w, arrY);
    pop();
  }
}
