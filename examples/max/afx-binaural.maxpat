{
  "patcher": {
    "fileversion": 1,
    "rect": [
      50,
      50,
      600,
      300
    ],
    "boxes": [
      {
        "box": {
          "id": "osc",
          "maxclass": "newobj",
          "text": "cycle~ 440",
          "patching_rect": [
            60,
            60,
            80,
            22
          ]
        }
      },
      {
        "box": {
          "id": "afx",
          "maxclass": "newobj",
          "text": "afx.binaural~ @mode cpu",
          "patching_rect": [
            60,
            120,
            200,
            22
          ]
        }
      },
      {
        "box": {
          "id": "dac",
          "maxclass": "newobj",
          "text": "ezdac~",
          "patching_rect": [
            60,
            180,
            60,
            22
          ]
        }
      },
      {
        "box": {
          "id": "msg",
          "maxclass": "message",
          "text": "pos 0 60 20 1.",
          "patching_rect": [
            300,
            120,
            120,
            20
          ]
        }
      }
    ],
    "lines": [
      {
        "patchline": {
          "source": [
            "osc",
            0
          ],
          "destination": [
            "afx",
            0
          ]
        }
      },
      {
        "patchline": {
          "source": [
            "afx",
            0
          ],
          "destination": [
            "dac",
            0
          ]
        }
      },
      {
        "patchline": {
          "source": [
            "afx",
            1
          ],
          "destination": [
            "dac",
            1
          ]
        }
      },
      {
        "patchline": {
          "source": [
            "msg",
            0
          ],
          "destination": [
            "afx",
            0
          ]
        }
      }
    ]
  }
}