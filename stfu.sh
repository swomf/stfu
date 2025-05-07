# The following evals passed arguments in an arbitrary
# subshell. It avoids the string sanitization bonanza
# that `someshell -i -c $ARGS` would require.
eval "$@" &
disown
