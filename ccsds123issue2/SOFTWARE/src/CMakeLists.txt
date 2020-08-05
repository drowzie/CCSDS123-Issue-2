add_executable(ccsds
  main.c
  utils/utilities.c

  predictor/localdiffrences.c
  predictor/localsum.c
  predictor/predictor.c
  predictor/unpredictor.c
  predictor/utilities.c
  predictor/weights.c

  cliparser/cli.c

  encoder/hash.c
  encoder/hybridencoder.c
  encoder/sampleAdaptive.c
  encoder/utilities.c

)

target_link_libraries(ccsds m)
