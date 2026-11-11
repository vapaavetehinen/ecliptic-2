module.exports = [
  {
    "type": "heading",
    "defaultValue": "Ecliptic 2"
  },
  {
    "type": "text",
    "defaultValue": "A port of Ecliptic by Konstantin Beliakov."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Display"
      },
      {
        "type": "toggle",
        "messageKey": "FlipHorizontal",
        "label": "Flip horizontally",
        "description": "Mirror the sky, as seen from the southern hemisphere.",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "EdgeBuffer",
        "label": "Keep planets off the screen edge",
        "description": "Shrinks the circle slightly so planets at the far left or right do not touch the edge.",
        "defaultValue": true,
        "capabilities": ["PLATFORM_EMERY"]
      },
      {
        "type": "toggle",
        "messageKey": "StarsByBrightness",
        "label": "Size stars by brightness",
        "description": "Draw the brightest stars larger. When off, every star is one pixel.",
        "defaultValue": false,
        "capabilities": ["COLOR"]
      },
      {
        "type": "toggle",
        "messageKey": "BgWhite",
        "label": "White background",
        "defaultValue": false,
        "capabilities": ["BW"]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Vibration"
      },
      {
        "type": "toggle",
        "messageKey": "VibrateHourly",
        "label": "Vibrate on the hour",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "VibrateDisconnect",
        "label": "Vibrate when the phone disconnects",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];
