#!/bin/lua

local path_sep = package.path:gmatch("/")() == "/" and "/" or "\\"

local info = "[info]:"
local warn = "[warn]:"
local err = "[error]:"

---@param sep string|nil
---@param ... string
---@return string
function concat(sep, ...)
  sep = sep or ""

  local args = {...}
  local result = ""

  for i=1, #args do
    result = result .. args[i] .. sep
  end

  result = result:sub(1, #result - 1)

  return result
end

function to_path(...)
  return concat(path_sep, ...)
end

---@param cmd string
---@param args string[]
---@return integer?
function cmd(cmd, args)
  local cargs = table.concat(args, " ")
  local c = concat(" ", cmd, cargs)
  local success, exitcode, code = os.execute(c)

  if success then
    print(concat(" ", info, success and "success" or "fail", "with exit code ", exitcode, tostring(code)))
  else
    print(concat(" ", err, success and "success" or "fail", "with exit code ", exitcode, tostring(code)))
  end

  return code
end

return {
  cmd("gcc", {"-o", to_path("build", "choi_disasm"), to_path(".", "src", "choi_disasm.c"), "-ggdb -Wall -Wextra -Werror -pedantic-errors -std=c99"}),
  cmd("gcc", {"-o", to_path("build", "choiasm"), to_path(".", "src", "choiasm.c"), "-ggdb -Wall -Wextra -Werror -pedantic-errors -std=c99"}),
  cmd("gcc", {"-o", to_path("build", "choivm"), to_path(".", "src", "choivm.c"), "-ggdb -Wall -Wextra -Werror -pedantic-errors -std=c99"})
}